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

struct bits
{
    unsigned offset;
    unsigned index;
    unsigned tag;
};

SC_MODULE(DirectMappedCache)
{

    // --- INPUT PORTS --- //

    sc_in<bool> clkCACHEIn;
    // Signal containing the address of the request
    sc_in<sc_uint<32>> cacheAddressCACHEIn;
    // Signal containing the data of the request, if it is a write request
    sc_in<sc_uint<32>> cacheWriteDataCACHEIn;
    // Signal indicating whether the request is a write request
    sc_in<bool> cacheWriteEnableCACHEIn;

    // Signal that contains the data read from the memory (full cache line)
    std::vector<sc_in<sc_uint<8>>> memoryReadDataCACHEIn;
    // Signal to indicate that the memory has finished processing the request
    sc_in<bool> memoryDoneCACHEIn;

    // --- OUTPUT PORTS --- //

    // Signal containing the data read from the cache (One Byte)
    sc_out<sc_uint<8>> cacheReadDataCACHEOut;
    // Signal to indicate that the cache has finished processing the request
    sc_out<bool> cacheDoneCACHEOut;

    // Signal containging the address of the request for the memory
    sc_out<sc_uint<32>> memoryAddressCACHEOut;
    // Signal containing the data of the request for the memory, if it is a write request
    sc_out<sc_uint<32>> memoryWriteDataCACHEOut;
    // Signal indicating whether the request for the memory is a write request
    sc_out<bool> memoryWriteEnableCACHEOut;
    // Signal to indicate that the memory can start processing the request
    sc_out<bool> memoryEnableCACHEOut;

    // --- INTERNAL VARIABLES --- //

    std::vector<CacheLine> cacheLines;
    // Statistics for the cache
    struct statistics statistics;
    // Struct to store the offset, index and tag bits
    struct bits bits;
    // Cache latency
    unsigned cacheLatency;
    // Cache Line Size
    unsigned cacheLineSize;

    void cacheAccess();
    void printCache();

    SC_CTOR(DirectMappedCache);

    DirectMappedCache(sc_module_name name, unsigned cacheSize, unsigned cacheLatency, unsigned cacheLineSize);
};

#endif
