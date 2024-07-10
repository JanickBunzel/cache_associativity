#include "directMappedCachePoly.h"
#include "cache.h"
#include <iostream>
#include <cmath>

DirectMappedCachePoly::DirectMappedCachePoly(sc_module_name name, unsigned cacheSize, unsigned cacheLatency, unsigned cacheLineSize)
    : Cache(name, cacheSize, cacheLatency, cacheLineSize)
{
    calculateBits(cacheSize, cacheLineSize);

    SC_THREAD(cacheAccess);
    dont_initialize();
    sensitive << clkCACHEIn.pos();

    printBits();
    printCache();
}

void DirectMappedCachePoly::cacheAccess()
{
    while (true)
    {
        wait(SC_ZERO_TIME);

        cacheDoneCACHEOut.write(false);
        this->statistics.accesses++;

        if (bits.offset < 0 || bits.index < 0 || bits.offset + bits.index > 31) {
            std::cerr << "Error: offsetBits and/or indexBits are out of valid range." << std::endl;
            break;
        }

        sc_uint<32> address = this->cacheAddressCACHEIn.read();
        sc_uint<32> offset = 0, index = 0, tag;

        if (bits.offset == 0 && bits.index == 0) {
            tag = address;
        } else if (bits.offset == 0) {
            index = address.range(bits.index - 1, 0);
            tag = address.range(31, bits.index);
        } else if (bits.index == 0) {
            offset = address.range(bits.offset - 1, 0);
            tag = address.range(31, bits.offset);
        } else {
            offset = address.range(bits.offset - 1, 0);
            index = address.range(bits.offset + bits.index - 1, bits.offset);
            tag = address.range(31, bits.offset + bits.index);
        }

        if (cacheWriteEnableCACHEIn.read() == 0)
        {
            this->statistics.reads++;
            std::vector<sc_uint<8>> rdata;
            for (unsigned i = 0; i < cacheLatency - 1; i++) {
                wait();
                wait(SC_ZERO_TIME);
            }
            const bool hit = cacheLines[index].read(tag, rdata);
            if (hit)
            {
                this->statistics.hits++;
            }
            else
            {
                this->statistics.misses++;
                memoryAddressCACHEOut.write(address);
                memoryWriteDataCACHEOut.write(false);
                memoryEnableCACHEOut.write(true);
                while (memoryDoneCACHEIn.read() == false)
                {
                    wait();
                    wait(SC_ZERO_TIME);
                }
                std::vector<sc_uint<8>> memoryData(cacheLineSize);
                for (unsigned i = 0; i < cacheLineSize; i++) {
                    memoryData[i] = memoryReadDataCACHEIn[i].read();
                }
                cacheLines[index].write(tag, memoryData);
                for (unsigned i = 0; i < cacheLineSize; ++i) {
                    cacheReadDataCACHEOut.write(memoryData[offset]);
                }
            }
        }
        else if (cacheWriteEnableCACHEIn.read() == 1)
        {
            this->statistics.writes++;
            for (unsigned i = 0; i < cacheLatency - 1; ++i) {
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
            for (unsigned i = 0; i < cacheLineSize; i++) {
                memoryData[i] = memoryReadDataCACHEIn[i].read();
            }
            cacheLines[index].write(tag, memoryData);
            for (unsigned i = 0; i < cacheLineSize; i++) {
                cacheReadDataCACHEOut.write(memoryData[offset]);
            }
            sc_uint<32> nextAddress = address;
            unsigned numBytes = 32 / 8;
            for (unsigned i = 1; i < numBytes; ++i) {
                nextAddress = address + i * cacheLineSize;
                sc_uint<32> nextIndex = (nextAddress >> bits.offset) % cacheLines.size();
                sc_uint<32> nextTag = nextAddress >> (bits.offset + bits.index);
                if (cacheLines[nextIndex].valid && cacheLines[nextIndex].tag == nextTag)
                {
                    cacheLines[nextIndex].valid = false;
                }
            }
        }
        memoryEnableCACHEOut.write(false);
        cacheDoneCACHEOut.write(true);
        printCache();
        wait();
    }
}

void DirectMappedCachePoly::printCache()
{
    std::cout << "Cache State:" << std::endl;
    std::cout << "----------------------------------------" << std::endl;
    std::cout << "Index\tTag\t\tValid\tLRU\tData (Hex/Binary)" << std::endl;
    std::cout << "----------------------------------------" << std::endl;

    for (unsigned i = 0; i < cacheLines.size(); ++i) {
        std::cout << i << "\t"
                  << cacheLines[i].tag.to_string(SC_HEX) << "\t"
                  << cacheLines[i].valid << "\t"
                  << cacheLines[i].lru << "\t";
        for (const auto& byte : cacheLines[i].data) {
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

void DirectMappedCachePoly::calculateBits(unsigned cacheSize, unsigned cacheLineSize)
{
    bits.offset = static_cast<unsigned int>(std::ceil(std::log2(static_cast<double>(cacheLineSize))));
    bits.index = static_cast<unsigned int>(std::ceil(std::log2(static_cast<double>(cacheSize))));
    bits.tag = 32 - bits.offset - bits.index;
}
