#include "cache.h"
#include <iostream>

Cache::Cache(sc_module_name name, unsigned cacheSize, unsigned cacheLatency, unsigned cacheLineSize)
    : sc_module(name), statistics({0,0,0,0,0}), bits({0,0,0}), cacheLatency(cacheLatency), cacheLineSize(cacheLineSize), memoryReadDataCACHEIn(cacheLineSize)
{
    for (unsigned i = 0; i < cacheSize; ++i)
    {
        cacheLines.emplace_back(cacheLineSize);
    }
}

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
