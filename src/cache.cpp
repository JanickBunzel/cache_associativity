#include "cache.h"
#include <iostream>

Cache::Cache(sc_module_name name, unsigned cachelines, unsigned cachelineSize, unsigned cacheLatency)
    : sc_module(name), statistics({0, 0, 0, 0, 0}), bits({0, 0, 0}), cachelineSize(cachelineSize),
      memoryReadDataCACHEIn(cachelineSize), cacheLatency(cacheLatency)
{
    for (unsigned i = 0; i < cachelines; ++i)
    {
        cachelinesArray.emplace_back(cachelineSize);
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

    // TODO comment
    wait();
    wait(SC_ZERO_TIME);

    // auf mem warten
    while (memoryDoneCACHEIn.read() == false)
    {
        wait();
        wait(SC_ZERO_TIME);
    }

    // cacheline von mem lesen und zurückgeben
    std::vector<sc_uint<8>> memoryData(cachelineSize);
    for (unsigned i = 0; i < cachelineSize; i++)
    {
        memoryData[i] = memoryReadDataCACHEIn[i].read();
    }

    // mem deaktivieren
    memoryEnableCACHEOut.write(false);

    return memoryData;
}

void Cache::writeMemoryData(sc_uint<32> address, sc_uint<32> writeData)
{
    // mem signals anlegen
    memoryAddressCACHEOut.write(address);
    memoryWriteDataCACHEOut.write(writeData);
    memoryWriteEnableCACHEOut.write(true);

    // mem starten
    memoryEnableCACHEOut.write(true);

    // TODO comment
    wait();
    wait(SC_ZERO_TIME);

    // auf mem warten
    while (memoryDoneCACHEIn.read() == false)
    {
        wait();
        wait(SC_ZERO_TIME);
    }

    // mem deaktivieren
    memoryEnableCACHEOut.write(false);
}

// Reads 4 bytes from the offset, using the second index if a second cacheline needs to be read -> Assuming cachelines being read are valid
sc_uint<32> Cache::readCacheData(unsigned offset, unsigned indexFirstCacheline, unsigned indexSecondCacheline)
{
    sc_uint<32> result = 0;

    unsigned currentByteIndex = 3;
    unsigned remaining = 4;

    // First cacheline
    for (unsigned i = 0; i < 4; i++)
    {
        if (offset + i >= cachelineSize)
        {
            break;
        }

        // Read from the first cacheline
        unsigned byte = cachelinesArray[indexFirstCacheline].getData()[offset + i];
        result += byte << (8 * currentByteIndex--);

        remaining--;
    }

    // Second cacheline
    for (unsigned i = 0; i < remaining; i++)
    {
        // Read from the second cacheline
        unsigned byte = cachelinesArray[indexSecondCacheline].getData()[i];
        result += byte << (8 * currentByteIndex--);
    }
    

    return result;
}

void Cache::writeCacheData(unsigned offset, unsigned indexFirstCacheline, unsigned indexSecondCacheline, sc_uint<32> writeData)
{
    unsigned remaining = 4;
    unsigned currentByteIndex = 0;

    // First cacheline
    std::vector<sc_uint<8>> cachlineData = cachelinesArray[indexFirstCacheline].getData();
    for (unsigned i = 0; i < 4; i++)
    {
        if (offset + i >= cachelineSize) { break; }

        // Write to corresponding byte in first cacheline
        cachlineData[offset + i] = writeData >> (8 * (3 - currentByteIndex++)) & 0xFF;

        remaining--;
    }
    cachelinesArray[indexFirstCacheline].setData(cachlineData);

    // Second cacheline
    cachlineData = cachelinesArray[indexSecondCacheline].getData();
    for (unsigned i = 0; i < remaining; i++)
    {
        // Write to corresponding byte in second cacheline
        cachlineData[i] = writeData >> (8 * (3 - currentByteIndex++)) & 0xFF;
    }
    cachelinesArray[indexSecondCacheline].setData(cachlineData);
}
