#include "fourwayMappedCache.h"
#include "cache.h"
#include <iostream>
#include <cmath>

// Flag passed from the rahmenprogramm (cache_simulaton parameter)
extern int printsEnabled;

FourwayMappedCache::FourwayMappedCache(sc_module_name name, unsigned cachelines, unsigned cachelineSize, unsigned cacheLatency)
    : Cache(name, cachelines, cachelineSize, cacheLatency)
{
    // Caclulate the number of bits for the offset, index and tag
    calculateBitCounts(cachelines, cachelineSize);

    // Set up the cache acess method, which is a SystemC thread and sensitive to the clock signal
    SC_THREAD(cacheAccess);
    dont_initialize();
    sensitive << clkCACHEIn.pos();

    // Print debug information
    printBits();
    printCache();
}

// Main method for every request
void FourwayMappedCache::cacheAccess()
{
    // While loop dependent on the clock signal
    while (true)
    {
        // Wait for the signals to be propagated
        wait(SC_ZERO_TIME);

        // Communicate that the cache is now processing the request
        cacheDoneCACHEOut.write(false);

        // Await the cache latency.
        for (unsigned i = 0; i < cacheLatency - 1; i++)
        {
            wait();
            wait(SC_ZERO_TIME);
        }

        // Update statistics
        this->statistics.accesses++;

        // Prepare the variables used during the cache access
        sc_uint<32> address = this->cacheAddressCACHEIn.read();
        bool hit = true;

        // Extract the offset, index and tag bits from the address
        extractBitsFromAddress(&bitValuesFirstAddress, bitCounts, address);

        // Indexes for the lines respective to a set
        int firstCachelineIndex = -1;
        int secondCachelineIndex = -1;

        // Check if the first cacheline is present in the Cache
        firstCachelineIndex = searchTagInSet(bitValuesFirstAddress.tag, bitValuesFirstAddress.index);
        if (firstCachelineIndex == -1)
        {
            // First cacheline is not present
            if (printsEnabled)
            {
                std::cout << "[Cache]: Miss in first cacheline" << std::endl;
            }
            hit = false;

            // If there is a free cacheline in the set, choose it
            firstCachelineIndex = searchFreeLineInSet(bitValuesFirstAddress.index);
            if (firstCachelineIndex == -1)
            {
                // No free cacheline found in the set, search for the least recently used cacheline
                firstCachelineIndex = searchLeastRecentlyUsedLineInSet(bitValuesFirstAddress.index);
                if (firstCachelineIndex == -1)
                {
                    if (printsEnabled)
                    {
                        std::cerr << "[Cache]: Error when trying to find the least recently used cacheline (no correct tag found, no free cacheline found, no LRU cacheline found)" << std::endl;
                    }
                    exit(1);
                }
            }

            // Fetch the cacheline from the memory and write to the cache
            getCacheline(bitValuesFirstAddress.index, firstCachelineIndex).setData(fetchMemoryData(address));
            getCacheline(bitValuesFirstAddress.index, firstCachelineIndex).setTag(bitValuesFirstAddress.tag);
            getCacheline(bitValuesFirstAddress.index, firstCachelineIndex).setValid(true);
        }

        // Update the LRU counter for the set of the first cacheline
        updateLruIndicesInSet(bitValuesFirstAddress.index, firstCachelineIndex);

        // Check if the second cacheline is present in the cache, if due to the offset second cacheline is needed
        if (bitValuesFirstAddress.offset + 4 > cachelineSize)
        {
            // The second address is calculated by adding the cachelineSize to the current address.
            sc_uint<32> secondAddress = (address + 4);
            extractBitsFromAddress(&bitValuesSecondAddress, bitCounts, secondAddress);

            // Check if the second cacheline is present in the Cache
            secondCachelineIndex = searchTagInSet(bitValuesSecondAddress.tag, bitValuesSecondAddress.index);
            if (secondCachelineIndex == -1)
            {
                // Second cacheline is not present
                if (printsEnabled)
                {
                    std::cout << "[Cache]: Miss in second cacheline" << std::endl;
                }
                hit = false;

                // If there is a free cacheline in the set, choose it
                secondCachelineIndex = searchFreeLineInSet(bitValuesSecondAddress.index);
                if (secondCachelineIndex == -1)
                {
                    // No free cacheline found in the set, search for the least recently used cacheline
                    secondCachelineIndex = searchLeastRecentlyUsedLineInSet(bitValuesSecondAddress.index);
                    if (firstCachelineIndex == -1)
                    {
                        if (printsEnabled)
                        {
                            std::cerr << "[Cache]: Error when trying to find the least recently used cacheline (no correct tag found, no free cacheline found, no LRU cacheline found)" << std::endl;
                        }
                        exit(1);
                    }
                }

                // Fetch the cacheline from the memory and write to the cache
                getCacheline(bitValuesSecondAddress.index, secondCachelineIndex).setData(fetchMemoryData(secondAddress));
                getCacheline(bitValuesSecondAddress.index, secondCachelineIndex).setTag(bitValuesSecondAddress.tag);
                getCacheline(bitValuesSecondAddress.index, secondCachelineIndex).setValid(true);
            }

            // Update the LRU counter for the set of the second cacheline
            updateLruIndicesInSet(bitValuesSecondAddress.index, secondCachelineIndex);
        }
        // The needed cacheline(s) are now stored in the cache and valid

        int firstCachelineIndexGlobal = firstCachelineIndex + 4 * bitValuesFirstAddress.index;
        int secondCachelineIndexGlobal = secondCachelineIndex + 4 * bitValuesSecondAddress.index;

        // Act depending on Write or Read
        if (cacheWriteEnableCACHEIn.read() == 0)
        {
            // READ request, update statistics
            this->statistics.reads++;

            // Read data from the cache and send it to the CPU
            cacheReadDataCACHEOut.write(readCacheData(bitValuesFirstAddress.offset, firstCachelineIndexGlobal, secondCachelineIndexGlobal));
        }
        else if (cacheWriteEnableCACHEIn.read() == 1)
        {
            // WRITE request, update statistics
            this->statistics.writes++;

            // Write given data to the cache
            writeCacheData(bitValuesFirstAddress.offset, firstCachelineIndexGlobal, secondCachelineIndexGlobal, cacheWriteDataCACHEIn.read());

            // Write given data to the memory
            writeMemoryData(address, cacheWriteDataCACHEIn.read());
        }

        // Access Done
        if (hit)
        {
            this->statistics.hits++;
        }
        else
        {
            this->statistics.misses++;
        }

        // Disable the memory
        memoryEnableCACHEOut.write(false);

        // Communicate that the cache is done via the output port
        cacheDoneCACHEOut.write(true);

        // Debug info of the cache state
        printCache();

        // Wait for the next clock cycle
        wait();
    }
}

void FourwayMappedCache::printCache()
{
    if (!printsEnabled)
    {
        return;
    }

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

void FourwayMappedCache::calculateBitCounts(unsigned cachelines, unsigned cachelineSize)
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

CacheLine &FourwayMappedCache::getCacheline(unsigned setIndex, unsigned cachelineIndex)
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
