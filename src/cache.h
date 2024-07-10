#ifndef CACHE_H
#define CACHE_H

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

class Cache : public sc_module
{
public:
    // --- INPUT PORTS --- //
    sc_in<bool> clkCACHEIn;
    sc_in<sc_uint<32>> cacheAddressCACHEIn;
    sc_in<sc_uint<32>> cacheWriteDataCACHEIn;
    sc_in<bool> cacheWriteEnableCACHEIn;
    std::vector<sc_in<sc_uint<8>>> memoryReadDataCACHEIn;
    sc_in<bool> memoryDoneCACHEIn;

    // --- OUTPUT PORTS --- //
    sc_out<sc_uint<8>> cacheReadDataCACHEOut;
    sc_out<bool> cacheDoneCACHEOut;
    sc_out<sc_uint<32>> memoryAddressCACHEOut;
    sc_out<sc_uint<32>> memoryWriteDataCACHEOut;
    sc_out<bool> memoryWriteEnableCACHEOut;
    sc_out<bool> memoryEnableCACHEOut;

    // --- INTERNAL VARIABLES --- //
    std::vector<CacheLine> cacheLines;
    struct statistics statistics;
    struct bits bits;
    unsigned cacheLatency;
    unsigned cacheLineSize;

    SC_HAS_PROCESS(Cache);

    Cache(sc_module_name name, unsigned cacheSize, unsigned cacheLatency, unsigned cacheLineSize);
    virtual ~Cache();

    virtual void cacheAccess() = 0;
    virtual void printCache() = 0;
    virtual void calculateBits(unsigned cacheSize, unsigned cacheLineSize) = 0;

    void printBits();
};

#endif
