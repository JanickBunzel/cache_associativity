#include "fourwayMappedCache.h"
#include "cache.h"
#include <iostream>
#include <cmath>

FourwayMappedCache::FourwayMappedCache(sc_module_name name, unsigned cachelines, unsigned cachelineSize, unsigned cacheLatency)
    : Cache(name, cachelines, cachelineSize, cacheLatency)
{
    calculateBits(cachelines, cachelineSize);

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
        int firstCachelineIndex = -1;

        Bits bitValuesSecondAdress = {0, 0, 0};
        int secondCachelineIndex = -1;

        // Extract the offset, index and tag bits from the address
        extractBitsFromAdress(&bitValues, bitCounts, address);

        // Await the cache latency.
        for (unsigned i = 0; i < cacheLatency - 1; i++)
        {
            wait();
            wait(SC_ZERO_TIME);
        }

        bool hit = true;
        sc_uint<32> nextIndex = 0;

        // Check if the first cacheline is present in the Cache
        firstCachelineIndex = searchTagInSet(bitValues.tag, bitValues.index);
        if (firstCachelineIndex == -1)
        {
            // First cacheline is not present
            std::cout << "[Cache]: Miss in first cacheline" << std::endl;
            hit = false;

            // If there is a free cacheline in the set, choose it
            firstCachelineIndex = searchFreeLineInSet(bitValues.index);
            if (firstCachelineIndex == -1)
            {
                // No free cacheline found in the set, search for the least recently used cacheline
                firstCachelineIndex = searchLeastRecentlyUsedLineInSet(bitValues.index);
                if (firstCachelineIndex == -1)
                {
                    std::cerr << "[Cache]: Error when trying to find the least recently used cacheline" << std::endl;
                    exit(1);
                }
            }

            // Update the LRU counter
            updateLruIndicesInSet(bitValues.index, firstCachelineIndex);

            // Fetch the cacheline from the memory and write to the cache
            getCacheline(bitValues.index, firstCachelineIndex).setData(fetchMemoryData(address));
            getCacheline(bitValues.index, firstCachelineIndex).setTag(bitValues.tag);
            getCacheline(bitValues.index, firstCachelineIndex).setValid(true);
        }

        // This bool cecks if the data lies in two rows by checking if the offset + 4 is greater than the cachelineSize.
        // This is done because the data is 4 bytes long and if the offset + 4 is greater than the cachelineSize, the data must lie in two rows.
        if (bitValues.offset + 4 > cachelineSize)
        {
            // The second address is calculated by adding the cachelineSize to the current address.
            sc_uint<32> secondAdress = (address + cachelineSize);

            extractBitsFromAdress(&bitValuesSecondAdress, bitCounts, secondAdress);

            // Check if the second cacheline is present in the Cache
            secondCachelineIndex = searchTagInSet(bitValuesSecondAdress.tag, bitValuesSecondAdress.index);
            if (secondCachelineIndex == -1)
            {
                // Second cacheline is not present
                std::cout << "[Cache]: Miss in second cacheline" << std::endl;
                hit = false;

                // If there is a free cacheline in the set, choose it
                secondCachelineIndex = searchFreeLineInSet(bitValuesSecondAdress.index);
                if (secondCachelineIndex == -1)
                {
                    // No free cacheline found in the set, search for the least recently used cacheline
                    secondCachelineIndex = searchLeastRecentlyUsedLineInSet(bitValuesSecondAdress.index);
                    if (secondCachelineIndex == -1)
                    {
                        std::cerr << "[Cache]: Error when trying to find the least recently used cacheline" << std::endl;
                        exit(1);
                    }
                }

                // Update the LRU counter
                updateLruIndicesInSet(bitValuesSecondAdress.index, secondCachelineIndex);

                // Fetch the cacheline from the memory and write to the cache
                getCacheline(bitValuesSecondAdress.index, secondCachelineIndex).setData(fetchMemoryData(secondAdress));
                getCacheline(bitValuesSecondAdress.index, secondCachelineIndex).setTag(bitValuesSecondAdress.tag);
                getCacheline(bitValuesSecondAdress.index, secondCachelineIndex).setValid(true);
            }
        }
        // The needed cacheline(s) are now stored in the cache and valid

        int firstCachelineIndexGlobal = firstCachelineIndex + 4 * bitValues.index;
        int secondCachelineIndexGlobal = secondCachelineIndex + 4 * bitValuesSecondAdress.index;

        // Act depending on Write or Read
        if (cacheWriteEnableCACHEIn.read() == 0)
        {
            // READ request, update statistics
            this->statistics.reads++;

            // Read data from the cache and send it to the CPU
            cacheReadDataCACHEOut.write(readCacheData(bitValues.offset, firstCachelineIndexGlobal, secondCachelineIndexGlobal));
        }
        else if (cacheWriteEnableCACHEIn.read() == 1)
        {
            // WRITE request, update statistics
            this->statistics.writes++;

            // Write given data to the cache
            writeCacheData(bitValues.offset, firstCachelineIndexGlobal, secondCachelineIndexGlobal, cacheWriteDataCACHEIn.read());

            // Write given data to the memory
            writeMemoryData(address, cacheWriteDataCACHEIn.read());
        }

        if (hit)
        {
            this->statistics.hits++;
        }
        else
        {
            this->statistics.misses++;
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

    for (unsigned i = 0; i < cachelinesArray.size(); i += 4)
    {
        unsigned set = i / 4; // Berechnet die Set-Nummer
        for (unsigned way = 0; way < 4; ++way)
        {
            CacheLine line = cachelinesArray[i + way];
            std::cout << set << "\t" << way << "\t"
                      << line.getTag().to_string(SC_HEX) << "\t"
                      << line.getValid() << "\t"
                      << static_cast<unsigned>(line.getLru() & 0x03) << "\t"; // Korrekte Darstellung der 2-Bit LRU-Zahl
            for (const auto &byte : line.getData())
            {
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

void FourwayMappedCache::calculateBits(unsigned cachelines, unsigned cachelineSize)
{
    bitCounts.offset = static_cast<unsigned int>(std::ceil(std::log2(static_cast<double>(cachelineSize))));

    unsigned numSets = std::ceil(static_cast<double>(cachelines) / 4);
    bitCounts.index = static_cast<unsigned int>(std::ceil(std::log2(static_cast<double>(numSets))));

    bitCounts.tag = 32 - bitCounts.offset - bitCounts.index;

    if (bitCounts.offset < 0 || bitCounts.index < 0 || bitCounts.offset + bitCounts.index > 31)
    {
        std::cerr << "Error: offsetBits and/or indexBits are out of valid range." << std::endl;
        exit(1);
    }
}

CacheLine& FourwayMappedCache::getCacheline(unsigned setIndex, unsigned cachelineIndex)
{
    return cachelinesArray[4 * setIndex + cachelineIndex];
}

int FourwayMappedCache::searchTagInSet(sc_uint<32> tag, unsigned setIndex)
{
    for (int i = 0; i < 4; i++)
    {
        if (getCacheline(setIndex, i).getTag() == tag && getCacheline(setIndex, i).getValid())
        {
            // Cacheline found with the right tag
            return i;
        }
    }

    return -1;
}

int FourwayMappedCache::searchFreeLineInSet(unsigned setIndex)
{
    for (int i = 0; i < 4; i++)
    {
        if (getCacheline(setIndex, i).getValid() == false)
        {
            // Found a free cacheline
            return i;
        }
    }

    return -1;
}

int FourwayMappedCache::searchLeastRecentlyUsedLineInSet(unsigned setIndex)
{
    for (int i = 0; i < 4; i++)
    {
        if (getCacheline(setIndex, i).getLru() == 0 && getCacheline(setIndex, i).getValid() == true)
        {
            return i;
        }
    }

    return -1;
}

void FourwayMappedCache::updateLruIndicesInSet(unsigned setIndex, unsigned cachelineIndexToUpdate)
{
    for (int i = 0; i < 4; i++)
    {
        if (getCacheline(setIndex, i).getLru() > getCacheline(setIndex, cachelineIndexToUpdate).getLru() && getCacheline(setIndex, i).getValid())
        {
            getCacheline(setIndex, i).setLru(getCacheline(setIndex, i).getLru() - 1);
        }
    }

    getCacheline(setIndex, cachelineIndexToUpdate).setLru(3);
}
