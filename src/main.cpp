#include <systemc.h>
#include "request.h"
#include "result.h"
#include "cpu.hpp"
#include "memory.h"
#include "cache.h"
#include "directMappedCache.h"
#include "fourwayMappedCache.h"

extern int cycles;
extern int directMapped;
extern unsigned cachelines;
extern unsigned cachelineSize;
extern unsigned cacheLatency;
extern unsigned memoryLatency;
extern size_t numRequests;
extern Request *requests;
extern const char *tracefile;

Result simulationResult;

int primitiveGateCount();

int sc_main(int argc, char *argv[])
{
    sc_clock clk("clk", 1, SC_NS);

    // ----- Cache ----- //
    Cache *cache;
    if (directMapped)
    {
        cache = new DirectMappedCache("cache_inst", cachelines, cachelineSize, cacheLatency);
    }
    else
    {
        cache = new FourwayMappedCache("cache_inst", cachelines, cachelineSize, cacheLatency);
    }
    cache->clkCACHEIn(clk);

    // ----- CPU ----- //
    Cpu cpu("cpu_inst", requests, numRequests);
    cpu.clkCPUIn(clk);

    // ----- Memory ----- //
    Memory memory("memory_inst", memoryLatency, cachelineSize);
    memory.clkMEMORYIn(clk);

    // ----- Signals Cache ----- //

    // - Cache done signal
    sc_signal<bool> cacheDoneSignal;
    cpu.cacheDoneCPUIn(cacheDoneSignal);
    cache->cacheDoneCACHEOut(cacheDoneSignal);
    // - Address of the next request
    sc_signal<sc_uint<32>> cacheAdressSignal;
    cpu.addressCPUOut(cacheAdressSignal);
    cache->cacheAddressCACHEIn(cacheAdressSignal);
    // - Value of the next request
    sc_signal<sc_uint<32>> cacheWriteDataSignal;
    cpu.writeDataCPUOut(cacheWriteDataSignal);
    cache->cacheWriteDataCACHEIn(cacheWriteDataSignal);
    // - Write or read request
    sc_signal<bool> cacheWriteEnableSignal;
    cpu.writeEnableCPUOut(cacheWriteEnableSignal);
    cache->cacheWriteEnableCACHEIn(cacheWriteEnableSignal);
    // - Cache read data
    sc_signal<sc_uint<32>> cacheReadDataSignal;
    cpu.cacheReadDataCPUIn(cacheReadDataSignal);
    cache->cacheReadDataCACHEOut(cacheReadDataSignal);

    // ----- Signals Memory ----- //

    // - Memory done signal
    sc_signal<bool> memoryDoneSignal;
    cache->memoryDoneCACHEIn(memoryDoneSignal);
    memory.doneMEMORYOut(memoryDoneSignal);
    // - Address of the memory request
    sc_signal<sc_uint<32>> memoryAddressSignal;
    cache->memoryAddressCACHEOut(memoryAddressSignal);
    memory.addressMEMORYIn(memoryAddressSignal);
    // - Value of the memory write request
    sc_signal<sc_uint<32>> memoryWriteDataSignal;
    cache->memoryWriteDataCACHEOut(memoryWriteDataSignal);
    memory.writeDataMEMORYIn(memoryWriteDataSignal);
    // - Write memory signal
    sc_signal<bool> memoryWriteEnableSignal;
    cache->memoryWriteEnableCACHEOut(memoryWriteEnableSignal);
    memory.writeEnableMEMORYIn(memoryWriteEnableSignal);
    // - Memory enable signal
    sc_signal<bool> memoryEnableSignal;
    cache->memoryEnableCACHEOut(memoryEnableSignal);
    memory.enableMEMORYIn(memoryEnableSignal);
    // - Memory read data
    std::vector<sc_signal<sc_uint<8>>> memoryReadDataSignals(cachelineSize);
    for (size_t i = 0; i < cachelineSize; i++)
    {
        cache->memoryReadDataCACHEIn[i](memoryReadDataSignals[i]);
        memory.readDataMEMORYOut[i](memoryReadDataSignals[i]);
    }

    // Create a tracefile to track all simulation signals
    sc_trace_file *tf;
    if (strcmp(tracefile, "") != 0)
    {
        tf = sc_create_vcd_trace_file(tracefile);
        sc_trace(tf, cacheDoneSignal, "cacheDoneSignal");
        sc_trace(tf, cacheAdressSignal, "cacheAdressSignal");
        sc_trace(tf, cacheWriteDataSignal, "cacheWriteDataSignal");
        sc_trace(tf, cacheWriteEnableSignal, "cacheWriteEnableSignal");
        sc_trace(tf, cacheReadDataSignal, "cacheReadDataSignal");
        sc_trace(tf, memoryAddressSignal, "memoryAddressSignal");
        sc_trace(tf, memoryWriteDataSignal, "memoryWriteDataSignal");
        sc_trace(tf, memoryWriteEnableSignal, "memoryWriteEnableSignal");
        sc_trace(tf, memoryEnableSignal, "memoryEnableSignal");
        sc_trace(tf, memoryDoneSignal, "memoryDoneSignal");
        for (size_t i = 0; i < cachelineSize; i++)
        {
            sc_trace(tf, memoryReadDataSignals[i], "memoryReadDataSignal" + std::to_string(i));
        }
    }

    // Start the simulation with the given number of cycles
    sc_start(cycles, SC_NS);

    // Close the tracefile
    if (strcmp(tracefile, "") != 0)
    {
        sc_close_vcd_trace_file(tf);
    }

    // Store the simulation results after the simulation has finished
    simulationResult.cycles = cpu.cpuStatistics.cycles > cycles ? SIZE_MAX : cpu.cpuStatistics.cycles; // If the simulation has not finished in the given number of cycles, return SIZE_MAX
    simulationResult.misses = cache->statistics.misses;
    simulationResult.hits = cache->statistics.hits;
    simulationResult.primitiveGateCount = primitiveGateCount();

    // Clean up dynamically allocated memory
    delete cache;

    return 0;
}

int primitiveGateCount()
{
    int primitiveGateCount = 0;

    primitiveGateCount = cachelineSize * 8 * 5;             // 5 gates per Flip Flop one FLip Flop for each bit of the cache line * 8 in byte
    primitiveGateCount *= cachelines;                       // multiply with the number of cache lines
    primitiveGateCount += cachelines * 2 ^ cachelineSize;   // control Unit for each cache line (multiplexer)
    primitiveGateCount += 150 * cachelines * cachelineSize; // 150 gates for each cache line for adding
    primitiveGateCount += 150 * cachelines;                 // 150 gates for each cache line for Hit or Miss structure

    if (directMapped)
    {
        primitiveGateCount += 2 ^ cachelines; // control Unit for each cache line (multiplexer)
    }
    else
    {
        int extra = 0;               // extra gates for 4 way associative
        extra += 2 ^ cachelines / 4; // control Unit for each cache line (multiplexer) for 4 way associative
        extra += cachelineSize * 8 * 5;
        extra *= cachelines;
        primitiveGateCount += extra;
        primitiveGateCount += 150 * cachelines / 4; // LRU Unit for each cache line for 4 way associative
    }

    return primitiveGateCount;
}