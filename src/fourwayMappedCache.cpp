#include "fourwayMappedCache.h"
#include "cache.h"
#include <iostream>
#include <cmath>

FourwayMappedCache::FourwayMappedCache(sc_module_name name, unsigned cacheLines, unsigned cacheLineSize, unsigned cacheLatency)
    : Cache(name, cacheLines, cacheLineSize, cacheLatency)
{
    calculateBits(cacheLines, cacheLineSize);

    SC_THREAD(cacheAccess);
    dont_initialize();
    sensitive << clkCACHEIn.pos();

    printBits();
    printCache();
}

void FourwayMappedCache::cacheAccess()
{
    while (true)
    {
        wait(SC_ZERO_TIME);

        cacheDoneCACHEOut.write(false);
        this->statistics.accesses++;

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
            for (unsigned i = 0; i < cacheLatency - 1; i++)
            {
                wait();
                wait(SC_ZERO_TIME);
            }
            unsigned setIndex = index * 4;
            int indexOfTag = searchTagInSet(tag, setIndex);

            // HIT
            if (indexOfTag != -1)
            {
                this->statistics.hits++;
                lruReplacement(indexOfTag);
            }
            // MISS
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
                for (unsigned i = 0; i < cacheLineSize; i++)
                {
                    memoryData[i] = memoryReadDataCACHEIn[i].read();
                }


                int indexOfFreeLine = searchFreeLineInSet(setIndex);
                // Found free line
                if (indexOfFreeLine != -1)
                {
                    cacheLinesArray[indexOfFreeLine].write(tag, memoryData);
                    lruReplacement(indexOfFreeLine);
                    //for (unsigned i = 0; i < cacheLineSize; ++i)
                    //{
                        cacheReadDataCACHEOut.write(memoryData[offset]);
                    //}
                }

                else
                {
                    int indexOfLruLine = searchLeastRecentlyUsedLine(setIndex);
                    cacheLinesArray[indexOfLruLine].write(tag, memoryData);
                    lruReplacement(indexOfLruLine);
                    cacheReadDataCACHEOut.write(memoryData[offset]);
                }
            }


        }
        else if (cacheWriteEnableCACHEIn.read() == 1)
        {
           //TODO: Implement write access
            std::cout << "Write access" << std::endl;
        }
        memoryEnableCACHEOut.write(false);
        cacheDoneCACHEOut.write(true);
        printCache();
        wait();
    }
}

void FourwayMappedCache::printCache()
{
    std::cout << "Cache State:" << std::endl;
    std::cout << "-----------------------------------------------------" << std::endl;
    std::cout << "Set\tWay\tTag\t\tValid\tLRU\tData (Hex/Binary)" << std::endl;
    std::cout << "-----------------------------------------------------" << std::endl;

    for (unsigned i = 0; i < cacheLinesArray.size(); i += 4) {
        unsigned set = i / 4; // Berechnet die Set-Nummer
        for (unsigned way = 0; way < 4; ++way) {
            const auto& line = cacheLinesArray[i + way];
            std::cout << set << "\t" << way << "\t"
                      << line.tag.to_string(SC_HEX) << "\t"
                      << line.valid << "\t"
                      << static_cast<unsigned>(line.lru & 0x03) << "\t"; // Korrekte Darstellung der 2-Bit LRU-Zahl
            for (const auto& byte : line.data) {
                std::cout << "("
                          << byte.to_string(SC_HEX) << " / "
                          << byte.to_string(SC_BIN) << ") ";
            }
            std::cout << std::endl;
        }
        std::cout << "-----------------------------------------------------" << std::endl;
    }

    std::cout << "Statistics:" << std::endl;
    std::cout << "Hits: " << statistics.hits << std::endl;
    std::cout << "Misses: " << statistics.misses << std::endl;
    std::cout << "Accesses: " << statistics.accesses << std::endl;
    std::cout << "Writes: " << statistics.writes << std::endl;
    std::cout << "Reads: " << statistics.reads << std::endl;
    std::cout << "-----------------------------------------------------" << std::endl;
}

void FourwayMappedCache::calculateBits(unsigned cacheLines, unsigned cacheLineSize)
{
    bits.offset = static_cast<unsigned int>(std::ceil(std::log2(static_cast<double>(cacheLineSize))));

    unsigned numSets = std::ceil(static_cast<double>(cacheLines) / 4);
    bits.index = static_cast<unsigned int>(std::ceil(std::log2(static_cast<double>(numSets))));

    bits.tag = 32 - bits.offset - bits.index;

    if (bits.offset < 0 || bits.index < 0 || bits.offset + bits.index > 31)
    {
        std::cerr << "Error: offsetBits and/or indexBits are out of valid range." << std::endl;
        exit(1);
    }
}

int FourwayMappedCache::searchTagInSet(sc_uint<32> tag, const unsigned setIndex)
{
    int indexOfTag = -1;
    for (int i = 0; i < 4; i++)
    {
        std::vector<sc_uint<8>> rdata;
        const bool hit = cacheLinesArray[setIndex + i].read(tag, rdata);
        if (hit)
        {
            indexOfTag = setIndex + i;
            break;
        }
    }
    return indexOfTag;
}

void FourwayMappedCache::lruReplacement(unsigned indexToUpdate)
{
    if (indexToUpdate >= cacheLinesArray.size())
    {
        std::cerr << "Error: You are trying to access an Element in lru[] that is out of bounds" << "[Index: " << indexToUpdate << "]" << std::endl;
        std::exit(EXIT_FAILURE);
    }

    unsigned cacheSet = indexToUpdate / 4;
    unsigned cacheSetIndex = 4 * cacheSet;

    for (int i = 0; i < 4; i++)
    {
        if (static_cast<unsigned>(cacheLinesArray[cacheSetIndex + i].lru) > static_cast<unsigned>(cacheLinesArray[indexToUpdate].lru))
        {
            cacheLinesArray[cacheSetIndex + i].lru = static_cast<unsigned>(cacheLinesArray[cacheSetIndex + i].lru) - 1;
        }
    }
    cacheLinesArray[indexToUpdate].updateLru(3);
}


int FourwayMappedCache::searchFreeLineInSet(unsigned setIndex)
{
    int indexOfFreeLine = -1;
    for (int i = 0; i < 4; i++)
    {
        if (cacheLinesArray[setIndex + i].valid == false)
        {
            indexOfFreeLine = setIndex + i;
            break;
        }
    }
    return indexOfFreeLine;
}

int FourwayMappedCache::searchLeastRecentlyUsedLine(unsigned setIndex)
{
    int indexOfLruLine = -1;
    for (int i = 0; i < 4; i++)
    {
        if (cacheLinesArray[setIndex + i].lru == 0)
        {
            indexOfLruLine = setIndex + i;
            break;
        }
    }
    return indexOfLruLine;
}
