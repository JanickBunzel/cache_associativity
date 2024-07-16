#include "cache.h"
#include <iostream>

Cache::Cache(sc_module_name name, unsigned cacheLines, unsigned cacheLineSize, unsigned cacheLatency)
    : sc_module(name), statistics({0, 0, 0, 0, 0}), bits({0, 0, 0}), cacheLineSize(cacheLineSize),
      memoryReadDataCACHEIn(cacheLineSize), cacheLatency(cacheLatency)
{
    for (unsigned i = 0; i < cacheLines; ++i)
    {
        cacheLinesArray.emplace_back(cacheLineSize);
    }
}

// Deconstructor
Cache::~Cache()
{
}

void Cache::printBits()
{
    std::cout << "Bits:" << std::endl;
    std::cout << "----------------------------------------" << std::endl;
    std::cout << "Offset: " << bits.offset << std::endl;
    std::cout << "Index: " << bits.index << std::endl;
    std::cout << "Tag: " << bits.tag << std::endl;
    std::cout << "----------------------------------------" << std::endl;
}

std::vector<sc_uint<8>> Cache::fetchMemoryData(sc_uint<32> address)
{
    // mem signals anlegen
    memoryAddressCACHEOut.write(address);
    memoryWriteEnableCACHEOut.write(false);

    // mem starten
    memoryEnableCACHEOut.write(true);

    // auf mem warten
    while (memoryDoneCACHEIn.read() == false)
    {
        wait();
        wait(SC_ZERO_TIME);
    }

    // cacheline von mem lesen und zurückgeben
    std::vector<sc_uint<8>> memoryData(cacheLineSize);
    for (unsigned i = 0; i < cacheLineSize; i++)
    {
        memoryData[i] = memoryReadDataCACHEIn[i].read();
    }

    return memoryData;
}

// Reads 4 bytes from the offset, using the second index if a second cacheline needs to be read
// Condition: Assuming cachelines being read are valid
sc_uint<32> Cache::readCacheData(unsigned offset, unsigned indexFirstCacheline, unsigned indexSecondCacheline)
{
    sc_uint<32> result = 0;

    unsigned currentByteIndex = 4;
    unsigned remaining = 4;

    // First cacheline
    for (unsigned i = 0; i < 4; i++)
    {
        if (offset + i >= cacheLineSize)
        {
            break;
        }

        // Read from the first cacheline
        unsigned byte = cacheLinesArray[indexFirstCacheline].getData()[offset + i];
        result += byte << (8 * currentByteIndex--);

        remaining--;
    }

    // Second cacheline
    for (unsigned i = 0; i < remaining; i++)
    {
        // Read from the second cacheline
        unsigned byte = cacheLinesArray[indexSecondCacheline].getData()[i];
        result += byte << (8 * currentByteIndex--);
    }


    return result;
}


void Cache::writeCacheData(unsigned offset, unsigned indexFirstCacheline, unsigned indexSecondCacheline, sc_uint<32> writeData)
{
    unsigned remaining = 4;
    unsigned currentByteIndex = 0;

    // First cacheline
    std::vector<sc_uint<8>> cachlineData = cacheLinesArray[indexFirstCacheline].getData();
    for (unsigned i = 0; i < 4; i++)
    {
        if (offset + i >= cacheLineSize) { break; }

        // Write to corresponding byte in first cacheline
        cachlineData[offset + i] = writeData >> (8 * (3 - currentByteIndex++)) & 0xFF;

        remaining--;
    }
    cacheLinesArray[indexFirstCacheline].setData(cachlineData);

    // Second cacheline
    cachlineData = cacheLinesArray[indexSecondCacheline].getData();
    for (unsigned i = 0; i < remaining; i++)
    {
        // Write to corresponding byte in second cacheline
        cachlineData[i] = writeData >> (8 * (3 - currentByteIndex++)) & 0xFF;
    }
    cacheLinesArray[indexSecondCacheline].setData(cachlineData);
}
