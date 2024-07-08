#ifndef DIRECTMAPPEDCACHE_H
#define DIRECTMAPPEDCACHE_H
#include <systemc.h>
#include <vector>
#include "cacheLine.h"


SC_MODULE(DirectMappedCache)
{
    // Cache statistics
    unsigned int cycles = 0;
    unsigned int misses = 0;
    unsigned int hits = 0;

    unsigned cacheLatency;
    unsigned memoryLatency;

    // Numbers of bits represented in an address:
    unsigned offsetBits; // - offset_bits: Bits for the position of a block in a cache line
    unsigned indexBits;  // - index_bits: Bits for the position of a set in the cache
    unsigned tagBits;    // - tag_bits: Bits for the tag of a cache line

    std::vector<CacheLine> cacheLines;

    // SystemC Input Ports:
    sc_in<bool> clk; // Clock Signal
    sc_in<bool> cpuDone;
    sc_in<sc_uint<32>> addr; // Address of the next request
    sc_in<sc_uint<32>> wdata; // Value of the next request
    sc_in<bool> we; // Write or read request

    SC_CTOR(DirectMappedCache);

    DirectMappedCache(sc_module_name name, unsigned cacheLineCount, unsigned cacheLineSize, unsigned cacheLatencyP, unsigned memoryLatencyP) : sc_module(name), cacheLatency(cacheLatencyP), memoryLatency(memoryLatencyP)
    {
        // Create cache lines
        for (unsigned i = 0; i < cacheLineCount; ++i)
        {
            cacheLines.emplace_back(sc_gen_unique_name("cacheLine"));
        }

        // Set bit format properties
        offsetBits = log2(cacheLineSize);
        indexBits = log2(cacheLineCount);
        tagBits = 32 - offsetBits - indexBits;

        // Set the thread for a request that accesses the cache
        SC_THREAD(cacheAccess);
        dont_initialize();
        sensitive << clk.pos();
    }

    void cacheAccess()
    {
    }

};

#endif //DIRECTMAPPEDCACHE_H
