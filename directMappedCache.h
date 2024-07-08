#ifndef DIRECTMAPPEDCACHE_H
#define DIRECTMAPPEDCACHE_H
#include <systemc.h>
#include <vector>
#include "cacheLine.h"

struct statistics
{
    unsigned hits;
    unsigned misses;
    unsigned accesses;
    unsigned writes;
    unsigned reads;
};

SC_MODULE(DirectMappedCache)
{
    SC_CTOR(DirectMappedCache);

    std::vector<CacheLine> cacheLines;
    struct statistics statistics;
    unsigned cacheLineCount;
    unsigned cacheLineSize;
    unsigned memoryLatency;
    unsigned cacheLatency;
    unsigned offsetBits;
    unsigned indexBits;
    unsigned tagBits;

    sc_in<sc_uint<32>> address;
    sc_in<sc_uint<32>> wdata;
    sc_out<sc_uint<32>> rdata;
    sc_out<bool> cacheDone;
    sc_in<bool> clock;
    sc_in<bool> we;

    DirectMappedCache(sc_module_name name, unsigned cacheLineCount, unsigned cacheLineSize, unsigned cacheLatency, unsigned memoryLatency)
    : sc_module(name), cacheLineCount(cacheLineCount), cacheLineSize(cacheLineSize), cacheLatency(cacheLatency), memoryLatency(memoryLatency)
    {
        for (unsigned i = 0; i < cacheLineCount; ++i) { cacheLines.emplace_back(); }

        offsetBits = log2(cacheLineSize);
        indexBits = log2(cacheLineCount);
        tagBits = 32 - offsetBits - indexBits;

        statistics = {0, 0, 0, 0, 0};

        SC_THREAD(cacheAccess);
        sensitive << clock.pos();
    }

    void cacheAccess();

};

#endif
