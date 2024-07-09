#include <systemc.h>
#include "request.h"
#include "result.h"
#include "cache.hpp"
#include "cpu.hpp"
#include <sstream>

extern int cycles;
extern int directMapped;
extern unsigned cacheLines;
extern unsigned cacheLineSize;
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

    // Initialize and connect cache and cpu to the clock
    Cache cache_inst("cache_inst", directMapped, cacheLines, cacheLineSize, cacheLatency, memoryLatency);
    cache_inst.clk(clk);
    Cpu cpu_inst("cpu_inst", numRequests, requests);
    cpu_inst.clk(clk);

    // Signal to indicate that the cache has finished processing the request
    sc_signal<bool> cache_done_signal;
    cache_inst.cacheDone(cache_done_signal);
    cpu_inst.cacheDone(cache_done_signal);
    cache_done_signal.write(true); // Start with true to indicate that the cache is ready to process the first request

    sc_signal<bool> cpu_done_signal;
    cpu_inst.cpuDone(cpu_done_signal);
    cache_inst.cpuDone(cpu_done_signal);
    cpu_done_signal.write(false);

    // Signals to pass requests to cache
    // - Address of the next request
    sc_signal<sc_uint<32>> addr_signal;
    cpu_inst.addr(addr_signal);
    cache_inst.addr(addr_signal);

    // - Value of the next request
    sc_signal<sc_uint<32>> data_signal;
    cpu_inst.wdata(data_signal);
    cache_inst.wdata(data_signal);

    // - Write or read request
    sc_signal<bool> we_signal;
    cpu_inst.we(we_signal);
    cache_inst.we(we_signal);

    // Create a tracefile to track the simulation signals
    sc_trace_file *tf;
    if (strcmp(tracefile, "") != 0) {
        tf = sc_create_vcd_trace_file(tracefile);
        sc_trace(tf, cache_done_signal, "cache_done_signal");
        sc_trace(tf, cpu_done_signal, "cpu_done_signal");
        sc_trace(tf, addr_signal, "addr_signal");
        sc_trace(tf, we_signal, "we_signal");
    }

    // Start the simulation with the given number of cycles
    sc_start(cycles, SC_NS);

    // Close the tracefile
    if (strcmp(tracefile, "") != 0) {
        sc_close_vcd_trace_file(tf);
    }

    // Store the simulation results after the simulation has finished
    simulationResult.cycles = cache_inst.cycles;
    simulationResult.misses = cache_inst.misses;
    simulationResult.hits = cache_inst.hits;
    simulationResult.primitiveGateCount = primitiveGateCount();

    return 0;
}

int primitiveGateCount()
{
    int primitiveGateCount = 0;

    primitiveGateCount = cacheLineSize * 8 * 5;             // 5 gates per Flip Flop one FLip Flop for each bit of the cache line * 8 in byte
    primitiveGateCount *= cacheLines;                       // multiply with the number of cache lines
    primitiveGateCount += cacheLines * 2 ^ cacheLineSize;   // control Unit for each cache line (multiplexer)
    primitiveGateCount += 150 * cacheLines * cacheLineSize; // 150 gates for each cache line for adding
    primitiveGateCount += 150 * cacheLines;                 // 150 gates for each cache line for Hit or Miss structure

    if (directMapped)
    {
        primitiveGateCount += 2 ^ cacheLines; // control Unit for each cache line (multiplexer)
    }
    else
    {
        int extra = 0;               // extra gates for 4 way associative
        extra += 2 ^ cacheLines / 4; // control Unit for each cache line (multiplexer) for 4 way associative
        extra += cacheLineSize * 8 * 5;
        extra *= cacheLines;
        primitiveGateCount += extra;
        primitiveGateCount += 150 * cacheLines / 4; // LRU Unit for each cache line for 4 way associative
    }

    return primitiveGateCount;
}