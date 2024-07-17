#include "directMappedCache.h"
#include "cache.h"
#include <iostream>
#include <cmath>

DirectMappedCache::DirectMappedCache(sc_module_name name, unsigned cachelines, unsigned cachelineSize, unsigned cacheLatency)
    : Cache(name, cachelines, cachelineSize, cacheLatency)
{
    calculateBits(cachelines, cachelineSize);

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

        bool hit = true;

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

            if (!(cachelinesArray[index].getTag() == tag) || !cachelinesArray[index].getValid())
            {
                std::cout << "Miss in first line (read)" << std::endl;
                hit = false;
                // If the data is not present in the cache, the data is fetched from the memory and stored in the corresponding cache line.
                cachelinesArray[index].setData(fetchMemoryData(address));
                cachelinesArray[index].setTag(tag);
                cachelinesArray[index].setValid(true);
            }

            // This bool cecks if the data lies in two rows by checking if the offset + 4 is greater than the cachelineSize.
            // This is done because the data is 4 bytes long and if the offset + 4 is greater than the cachelineSize, the data must lie in two rows.
            if (offset + 4 > cachelineSize)
            {
                // The next address is calculated by adding the cachelineSize to the current address.
                sc_uint<32> nextAdress = (address + cachelineSize);
                // Extracting the index from the next address.
                nextIndex = nextAdress.range(bits.offset + bits.index - 1, bits.offset);
                // Extracting the tag from the next address.
                sc_uint<32> nextTag = nextAdress.range(31, bits.offset + bits.index);

                // The second cacheline also needs to have the right tag and is valid
                if (cachelinesArray[nextIndex].getTag() != nextTag || !cachelinesArray[nextIndex].getValid())
                {
                    std::cout << "Miss in second line (read)" << std::endl;
                    hit = false;
                    
                    // Fetching the data from the next row and storing it in the corresponding cache line.
                    cachelinesArray[nextIndex].setData(fetchMemoryData(nextAdress));
                    cachelinesArray[nextIndex].setTag(nextTag);
                    cachelinesArray[nextIndex].setValid(true);
                }
            }

            // The data is read from the cache and sent to the CPU.
            cacheReadDataCACHEOut.write(readCacheData(offset, index, nextIndex.to_uint()));
        }


        else if (cacheWriteEnableCACHEIn.read() == 1)
        {
            this->statistics.writes++;

            sc_uint<32> nextIndex = 0;

            for (unsigned i = 0; i < cacheLatency - 1; ++i)
            {
                wait();
                wait(SC_ZERO_TIME);
            }

            if (!(cachelinesArray[index].getTag() == tag) || !cachelinesArray[index].getValid())
            {
                std::cout << "Miss in first line (write)" << std::endl;
                hit = false;
                // If the data is not present in the cache, the data is fetched from the memory and stored in the corresponding cache line.
                cachelinesArray[index].setData(fetchMemoryData(address));
                cachelinesArray[index].setTag(tag);
                cachelinesArray[index].setValid(true);
            }

            // This bool cecks if the data lies in two rows by checking if the offset + 4 is greater than the cachelineSize.
            // This is done because the data is 4 bytes long and if the offset + 4 is greater than the cachelineSize, the data must lie in two rows.
            if (offset + 4 > cachelineSize)
            {
                // The next address is calculated by adding the cachelineSize to the current address.
                sc_uint<32> nextAdress = (address + cachelineSize);
                // Extracting the index from the next address.
                nextIndex = nextAdress.range(bits.offset + bits.index - 1, bits.offset);
                // Extracting the tag from the next address.
                sc_uint<32> nextTag = nextAdress.range(31, bits.offset + bits.index);

                // The second cacheline also needs to have the right tag and is valid
                if (cachelinesArray[nextIndex].getTag() != nextTag || !cachelinesArray[nextIndex].getValid())
                {
                    std::cout << "Miss in second line (write)" << std::endl;
                    hit = false;
                    
                    // Fetching the data from the next row and storing it in the corresponding cache line.
                    cachelinesArray[nextIndex].setData(fetchMemoryData(nextAdress));
                    cachelinesArray[nextIndex].setTag(nextTag);
                    cachelinesArray[nextIndex].setValid(true);
                }
            }

            // Write given data to the cache
            writeCacheData(offset, index, nextIndex.to_uint(), cacheWriteDataCACHEIn.read());
            
            // Write given data to the memory
            writeMemoryData(address, cacheWriteDataCACHEIn.read());
        }
        
        if (hit) { this->statistics.hits++; }
        else { this->statistics.misses++; }

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

    for (unsigned i = 0; i < cachelinesArray.size(); ++i)
    {
        std::cout << i << "\t"
                  << cachelinesArray[i].getTag().to_string(SC_HEX) << "\t"
                  << cachelinesArray[i].getValid() << "\t"
                  << cachelinesArray[i].getLru() << "\t";
        for (const auto &byte : cachelinesArray[i].getData())
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

void DirectMappedCache::calculateBits(unsigned cachelines, unsigned cachelineSize)
{
    bits.offset = static_cast<unsigned int>(std::ceil(std::log2(static_cast<double>(cachelineSize))));
    bits.index = static_cast<unsigned int>(std::ceil(std::log2(static_cast<double>(cachelines))));
    bits.tag = 32 - bits.offset - bits.index;
    
    if (bits.offset < 0 || bits.index < 0 || bits.offset + bits.index > 31)
    {
        std::cerr << "Error: offsetBits and/or indexBits are out of valid range." << std::endl;
        exit(1);
    }
}
