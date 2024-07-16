#include "cache.h"
#include <iostream>

Cache::Cache(sc_module_name name, unsigned cacheLines, unsigned cacheLineSize, unsigned cacheLatency)
    : sc_module(name), statistics({0, 0, 0, 0, 0}), bits({0, 0, 0}), cacheLineSize(cacheLineSize), memoryReadDataCACHEIn(cacheLineSize), cacheLatency(cacheLatency)
{
    for (unsigned i = 0; i < cacheLines; ++i)
    {
        cacheLinesArray.emplace_back(cacheLineSize);
    }
}

// Deconstructor
Cache::~Cache() {}

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