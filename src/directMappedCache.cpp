#include <cmath>
#include <iomanip>
#include <iostream>
#include "cache.h"
#include "directMappedCache.h"

// Global variables provided by the rahmenprogramm (cache_simulaton option), specifies how the debug information is printed
extern int printsLevel;
extern char *highlightDataColor;
extern char *resetColor;

DirectMappedCache::DirectMappedCache(sc_module_name name, unsigned cachelines, unsigned cachelineSize, unsigned cacheLatency)
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
void DirectMappedCache::cacheAccess()
{
    // While loop dependent on the clock signal
    while (true)
    {
        // Wait for the signals to be propagated
        wait(SC_ZERO_TIME);

        // Communicate that the cache is now processing the request
        cacheDoneCACHEOut.write(false);

        // Update statistics
        this->statistics.accesses++;

        // Await the cache latency.
        for (unsigned i = 0; i < cacheLatency - 1; i++)
        {
            wait();
            wait(SC_ZERO_TIME);
        }

        // Prepare the variables used during the cache access
        sc_uint<32> address = this->cacheAddressCACHEIn.read();
        bool hit = true;

        // Extract the actual offset, index and tag bits from the address
        extractBitsFromAddress(&bitValuesFirstAddress, bitCounts, address);

        // Check if the first cacheline is present in the cache
        if (!(cachelinesArray[bitValuesFirstAddress.index].getTag() == bitValuesFirstAddress.tag) || !cachelinesArray[bitValuesFirstAddress.index].getValid())
        {
            if (printsLevel >= 2)
            {
                std::cout << "[Cache]: Miss in first cacheline" << std::endl;
            }
            hit = false;

            // Fetch the cacheline from the memory and write to the cache
            cachelinesArray[bitValuesFirstAddress.index].setData(fetchMemoryData(address));
            cachelinesArray[bitValuesFirstAddress.index].setTag(bitValuesFirstAddress.tag);
            cachelinesArray[bitValuesFirstAddress.index].setValid(true);
        }

        // Check if the second cacheline is present in the cache, if due to the offset second cacheline is needed
        if (bitValuesFirstAddress.offset + 4 > cachelineSize)
        {
            // The next address is calculated by adding the cachelineSize to the current address.
            sc_uint<32> secondAddress = (address + 4);
            extractBitsFromAddress(&bitValuesSecondAddress, bitCounts, secondAddress);

            // Check if the second cacheline is present in the Cache
            if (cachelinesArray[bitValuesSecondAddress.index].getTag() != bitValuesSecondAddress.tag || !cachelinesArray[bitValuesSecondAddress.index].getValid())
            {
                if (printsLevel >= 2)
                {
                    std::cout << "[Cache]: Miss in second cacheline" << std::endl;
                }
                hit = false;

                // Fetch the second cacheline from the memory and write to the cache
                cachelinesArray[bitValuesSecondAddress.index].setData(fetchMemoryData(secondAddress));
                cachelinesArray[bitValuesSecondAddress.index].setTag(bitValuesSecondAddress.tag);
                cachelinesArray[bitValuesSecondAddress.index].setValid(true);
            }
        }
        // The needed cacheline(s) are now stored in the cache and valid

        // Act depending on Write or Read
        if (cacheWriteEnableCACHEIn.read() == 0)
        {
            // READ request, update statistics
            this->statistics.reads++;

            // Read data from the cache and send it to the CPU
            cacheReadDataCACHEOut.write(readCacheData(bitValuesFirstAddress.offset, bitValuesFirstAddress.index, bitValuesSecondAddress.index));
        }
        else if (cacheWriteEnableCACHEIn.read() == 1)
        {
            // WRITE request, update statistics
            this->statistics.writes++;

            // Write given data to the cache
            writeCacheData(bitValuesFirstAddress.offset, bitValuesFirstAddress.index, bitValuesSecondAddress.index, cacheWriteDataCACHEIn.read());

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

void DirectMappedCache::printCache()
{
    if (printsLevel == 0)
    {
        return;
    }

    std::cout << "Cache State:" << std::endl;
    std::cout << "----------------------------------------" << std::endl;
    std::cout << "Index\tTag\t\tValid\tLRU\tData" << std::endl;
    std::cout << "----------------------------------------" << std::endl;

    for (unsigned i = 0; i < cachelinesArray.size(); ++i)
    {
        if (cachelinesArray[i].getValid() == 1 && printsLevel > 0)
        {
            std::cout << highlightDataColor; // Highlight valid cache lines
        }

        // Print the cache line
        std::cout << i << "\t"
                  << cachelinesArray[i].getTag().to_string(SC_HEX) << "\t"
                  << cachelinesArray[i].getValid() << "\t"
                  << cachelinesArray[i].getLru() << "\t";
        // Data
        for (const auto &byte : cachelinesArray[i].getData())
        {
            std::cout << "0x" << std::setw(2) << std::setfill('0') << std::hex << (0xFF & byte) << std::dec << " ";
        }

        if (cachelinesArray[i].getValid() == 1 && printsLevel > 0)
        {
            std::cout << resetColor; // Reset the color
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

void DirectMappedCache::calculateBitCounts(unsigned cachelines, unsigned cachelineSize)
{
    bitCounts.offset = static_cast<unsigned int>(std::ceil(std::log2(static_cast<double>(cachelineSize))));
    bitCounts.index = static_cast<unsigned int>(std::ceil(std::log2(static_cast<double>(cachelines))));
    bitCounts.tag = 32 - bitCounts.offset - bitCounts.index;

    if (bitCounts.offset < 0 || bitCounts.index < 0 || bitCounts.offset + bitCounts.index > 31)
    {
        std::cerr << "Error: offsetBits and/or indexBits are out of valid range." << std::endl;
        exit(1);
    }
}
