#ifndef CACHE_H
#define CACHE_H

#include <systemc.h>
#include <vector>
#include "cacheLine.h"

typedef struct Statistics
{
    unsigned hits;
    unsigned misses;
    unsigned accesses;
    unsigned writes;
    unsigned reads;
} Statistics;

typedef struct Bits
{
    unsigned offset;
    unsigned index;
    unsigned tag;
} Bits;

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
    sc_out<sc_uint<32>> cacheReadDataCACHEOut;
    sc_out<bool> cacheDoneCACHEOut;
    sc_out<sc_uint<32>> memoryAddressCACHEOut;
    sc_out<sc_uint<32>> memoryWriteDataCACHEOut;
    sc_out<bool> memoryWriteEnableCACHEOut;
    sc_out<bool> memoryEnableCACHEOut;

    // --- INTERNAL VARIABLES --- //
    std::vector<CacheLine> cachelinesArray;
    Statistics statistics;
    Bits bitCounts;
    Bits bitValuesFirstAddress;
    Bits bitValuesSecondAddress;
    unsigned cachelineSize;
    unsigned cacheLatency;

    SC_HAS_PROCESS(Cache);

    Cache(sc_module_name name, unsigned cachelines, unsigned cachelineSize, unsigned cacheLatency);
    virtual ~Cache();

    virtual void cacheAccess() = 0;
    virtual void printCache() = 0;
    virtual void calculateBitCounts(unsigned cachelines, unsigned cachelineSize) = 0;

    // General Cache methods for all types
    void printBits();
    void extractBitsFromAddress(Bits *bitValues, Bits bitCounts, sc_uint<32> address);

    // Method to Read and Write data from the Memory
    std::vector<sc_uint<8>> fetchMemoryData(sc_uint<32> address);
    void writeMemoryData(sc_uint<32> address, sc_uint<32> data);

    // Method to Read and Write data from the Cache
    sc_uint<32> readCacheData(unsigned offset, unsigned indexFirstCacheline, unsigned indexSecondCacheline);
    void writeCacheData(unsigned offset, unsigned indexFirstCacheline, unsigned indexSecondCacheline, sc_uint<32> writeData);
};

#endif
