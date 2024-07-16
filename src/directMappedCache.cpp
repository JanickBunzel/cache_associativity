#include "directMappedCache.h"
#include "cache.h"
#include <iostream>
#include <cmath>

DirectMappedCache::DirectMappedCache(sc_module_name name, unsigned cacheLines, unsigned cacheLineSize, unsigned cacheLatency)
    : Cache(name, cacheLines, cacheLineSize, cacheLatency)
{
    calculateBits(cacheLines, cacheLineSize);

    SC_THREAD(cacheAccess);
    dont_initialize();
    sensitive << clkCACHEIn.pos();

    printBits();
    printCache();
}

void DirectMappedCache::cacheAccess()
{
    while (true)
    {
        wait(SC_ZERO_TIME);

        cacheDoneCACHEOut.write(false);
        this->statistics.accesses++;

        sc_uint<32> address = this->cacheAddressCACHEIn.read();
        sc_uint<32> offset = 0, index = 0, tag;

        if (bits.offset == 0 && bits.index == 0)
        {
            tag = address;
        }
        else if (bits.offset == 0)
        {
            index = address.range(bits.index - 1, 0);
            tag = address.range(31, bits.index);
        }
        else if (bits.index == 0)
        {
            offset = address.range(bits.offset - 1, 0);
            tag = address.range(31, bits.offset);
        }
        else
        {
            offset = address.range(bits.offset - 1, 0);
            index = address.range(bits.offset + bits.index - 1, bits.offset);
            tag = address.range(31, bits.offset + bits.index);
        }

        if (cacheWriteEnableCACHEIn.read() == 0)
        {
            this->statistics.reads++;
            std::vector<sc_uint<8>> rdata;
            sc_uint<32> nextIndex = 0;

            // Await the cache latency.
            for (unsigned i = 0; i < cacheLatency - 1; i++)
            {
                wait();
                wait(SC_ZERO_TIME);
            }

            bool hit = true;
            if (!cacheLinesArray[index].getTag() == tag || !cacheLinesArray[index].getValid())
            {
                hit = false;
                // If the data is not present in the cache, the data is fetched from the memory and stored in the corresponding cache line.
                cacheLinesArray[index].setData(fetchMemoryData(address));
                cacheLinesArray[index].setTag(tag);
                cacheLinesArray[index].setValid(true);
            }

            // This bool cecks if the data lies in two rows by checking if the offset + 4 is greater than the cacheLineSize.
            // This is done because the data is 4 bytes long and if the offset + 4 is greater than the cacheLineSize, the data must lie in two rows.
            if (offset + 4 > cacheLineSize)
            {
                hit = false;
                // The next address is calculated by adding the cacheLineSize to the current address.
                sc_uint<32> nextAdress = (address + cacheLineSize);
                // Extracting the index from the next address.
                nextIndex = nextAdress.range(bits.offset + bits.index - 1, bits.offset);
                // Extracting the tag from the next address.
                sc_uint<32> nextTag = nextAdress.range(31, bits.offset + bits.index);
                // Fetching the data from the next row and storing it in the corresponding cache line.
                cacheLinesArray[nextIndex].setData(fetchMemoryData(nextAdress));
                cacheLinesArray[nextIndex].setTag(nextTag);
                cacheLinesArray[nextIndex].setValid(true);
            }

            if (hit) { this->statistics.hits++; }
            else { this->statistics.misses++; }

            // The data is read from the cache and sent to the CPU.
            cacheReadDataCACHEOut.write(readCacheData(offset, index, nextIndex.to_uint()));
        }


        else if (cacheWriteEnableCACHEIn.read() == 1)
        {
            this->statistics.writes++;

            for (unsigned i = 0; i < cacheLatency - 1; ++i)
            {
                wait();
                wait(SC_ZERO_TIME);
            }
            memoryAddressCACHEOut.write(address);
            memoryWriteDataCACHEOut.write(cacheWriteDataCACHEIn.read());
            memoryWriteEnableCACHEOut.write(true);
            memoryEnableCACHEOut.write(true);
            while (memoryDoneCACHEIn.read() == false)
            {
                wait();
                wait(SC_ZERO_TIME);
            }
            std::vector<sc_uint<8>> memoryData(cacheLineSize);
            for (unsigned i = 0; i < cacheLineSize; i++)
            {
                memoryData[i] = memoryReadDataCACHEIn[i].read();
            }
            cacheLinesArray[index].write(tag, memoryData);
            for (unsigned i = 0; i < cacheLineSize; i++)
            {
                cacheReadDataCACHEOut.write(memoryData[offset]);
            }
            sc_uint<32> nextAddress = address;
            unsigned numBytes = 32 / 8;
            for (unsigned i = 1; i < numBytes; ++i)
            {
                nextAddress = address + i * cacheLineSize;
                sc_uint<32> nextIndex = (nextAddress >> bits.offset) % cacheLinesArray.size();
                sc_uint<32> nextTag = nextAddress >> (bits.offset + bits.index);
                if (cacheLinesArray[nextIndex].valid && cacheLinesArray[nextIndex].tag == nextTag)
                {
                    cacheLinesArray[nextIndex].valid = false;
                }
            }
        }
        memoryEnableCACHEOut.write(false);
        cacheDoneCACHEOut.write(true);
        printCache();
        wait();
    }
}

void DirectMappedCache::printCache()
{
    std::cout << "Cache State:" << std::endl;
    std::cout << "----------------------------------------" << std::endl;
    std::cout << "Index\tTag\t\tValid\tLRU\tData (Hex/Binary)" << std::endl;
    std::cout << "----------------------------------------" << std::endl;

    for (unsigned i = 0; i < cacheLinesArray.size(); ++i)
    {
        std::cout << i << "\t"
                  << cacheLinesArray[i].tag.to_string(SC_HEX) << "\t"
                  << cacheLinesArray[i].valid << "\t"
                  << cacheLinesArray[i].lru << "\t";
        for (const auto &byte : cacheLinesArray[i].data)
        {
            std::cout << "("
                      << byte.to_string(SC_HEX) << " / "
                      << byte.to_string(SC_BIN) << ") ";
        }
        std::cout << std::endl;
    }

    std::cout << "----------------------------------------" << std::endl;
    std::cout << "Statistics:" << std::endl;
    std::cout << "Hits: " << statistics.hits << std::endl;
    std::cout << "Misses: " << statistics.misses << std::endl;
    std::cout << "Accesses: " << statistics.accesses << std::endl;
    std::cout << "Writes: " << statistics.writes << std::endl;
    std::cout << "Reads: " << statistics.reads << std::endl;
    std::cout << "----------------------------------------" << std::endl;
}

void DirectMappedCache::calculateBits(unsigned cacheLines, unsigned cacheLineSize)
{
    bits.offset = static_cast<unsigned int>(std::ceil(std::log2(static_cast<double>(cacheLineSize))));
    bits.index = static_cast<unsigned int>(std::ceil(std::log2(static_cast<double>(cacheLines))));
    bits.tag = 32 - bits.offset - bits.index;
    
    if (bits.offset < 0 || bits.index < 0 || bits.offset + bits.index > 31)
    {
        std::cerr << "Error: offsetBits and/or indexBits are out of valid range." << std::endl;
        exit(1);
    }
}

