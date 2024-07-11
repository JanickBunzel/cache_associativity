#include "cache.h"
#include <iostream>

Cache::Cache(sc_module_name name, unsigned cacheLines, unsigned cacheLineSize, unsigned cacheLatency)
    : sc_module(name), statistics({0,0,0,0,0}), bits({0,0,0}), cacheLineSize(cacheLineSize), memoryReadDataCACHEIn(cacheLineSize), cacheLatency(cacheLatency)
{
    for (unsigned i = 0; i < cacheLines; ++i)
    {
        cacheLinesArray.emplace_back(cacheLineSize);
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
