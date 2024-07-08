#ifndef DIRECTMAPPEDCACHE_H
#define DIRECTMAPPEDCACHE_H
#include <systemc.h>
#include "cacheLine.h"

SC_MODULE(DirectMappedCache)
{
    std::vector<CacheLine> cacheLines;
    unsigned cachLatency;
    unsigned memoryLatency;

    DirectMappedCache(sc_module_name name, unsigned cacheLines, unsigned chacheLineSize, unsigned cacheLatency, unsigned memoryLatency)
    : sc_module(name), cacheLatency(cacheLatency), memoryLatency(memoryLatency)
    {
       for
    }
}

#endif //DIRECTMAPPEDCACHE_H
