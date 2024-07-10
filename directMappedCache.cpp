#include "directMappedCache.h"

DirectMappedCache::DirectMappedCache(sc_module_name name, unsigned cacheSize,const unsigned cacheLatency, const unsigned cacheLineSize)
: sc_module(name), statistics({0,0,0,0,0}), bits({0,0,0}), cacheLatency(cacheLatency), cacheLineSize(cacheLineSize), memoryReadDataCACHEIn(cacheLineSize)
{
    bits.offset = log(cacheLineSize);
    bits.index = log(cacheSize);
    bits.tag = 32 - bits.offset - bits.index;

    for (unsigned i = 0; i < cacheSize; ++i)
    {
        cacheLines.emplace_back(cacheLineSize);
    }

    SC_THREAD(cacheAccess);
    dont_initialize();
    sensitive << clkCACHEIn.pos();
}

void DirectMappedCache::cacheAccess()
{
    while (true)
    {
        wait(SC_ZERO_TIME);

        cacheDoneCACHEOut.write(false);
        this->statistics.accesses = this->statistics.accesses + 1;

        std::cout << "[Cache] Accesses: " << this->statistics.accesses << std::endl;

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

        std::cout << cacheWriteEnableCACHEIn.read() << std::endl;
        std::cout << address << std::endl;
        std::cout << offset << std::endl;
        std::cout << index << std::endl;
        std::cout << tag << std::endl;
        std::cout << cacheWriteDataCACHEIn.read() << std::endl;

        if (cacheWriteEnableCACHEIn.read() == 0)
        {
            this->statistics.reads = this->statistics.reads + 1;
            std::vector<sc_uint<8>> rdata;
            for (unsigned i = 0; i < cacheLatency - 1; i++) {
                std::cout << "[Cache] Waiting cacheLatency" << std::endl;
                wait();
                wait(SC_ZERO_TIME);
            }
            const bool hit = cacheLines[index].read(tag, rdata);
            if (hit)
            {
                this->statistics.hits++;
                std::cout << "[Cache] Hit" << std::endl;
            }
            else
            {
                this->statistics.misses++;
                std::cout << "[Cache] Miss" << std::endl;
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
            this->statistics.writes = this->statistics.writes + 1;
            std::cout << "[Cache] Write" << std::endl;
            for (unsigned i = 0; i < cacheLatency - 1; ++i) {
                std::cout << "[Cache] Waiting cacheLatency" << std::endl;
                wait();
                wait(SC_ZERO_TIME);
            }
            memoryAddressCACHEOut.write(address);
            memoryWriteDataCACHEOut.write(cacheWriteDataCACHEIn.read());
            memoryWriteEnableCACHEOut.write(true);
            memoryEnableCACHEOut.write(true);
            std::cout << "[Cache] Writing to memory" << std::endl;
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
        }
        memoryEnableCACHEOut.write(false);
        cacheDoneCACHEOut.write(true);
        std::cout << "[Cache] Done" << std::endl;
        printCache();
        wait();
    }
}

void DirectMappedCache::printCache()
{
    std::cout << "Cache State:" << std::endl;
    std::cout << "----------------------------------------" << std::endl;
    std::cout << "Index\tTag\t\tValid\tLRU\tData" << std::endl;
    std::cout << "----------------------------------------" << std::endl;

    for (unsigned i = 0; i < cacheLines.size(); ++i) {
        std::cout << i << "\t"
                  << cacheLines[i].tag.to_string(SC_HEX) << "\t"
                  << cacheLines[i].valid << "\t"
                  << cacheLines[i].lru << "\t";
        for (const auto& byte : cacheLines[i].data) {
            std::cout << byte.to_string(SC_HEX) << " ";
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