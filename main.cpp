#include <systemc.h>
#include "request.h"
#include "result.h"
#include "cache.hpp"
#include "testbench.hpp"
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

int sc_main(int argc, char *argv[])
{
    // TODO: Implement Tracefile

    sc_clock clk("clk", 1, SC_NS);

    // Initialize and connect cache and testbench to the clock
    Cache cache_inst("cache_inst", directMapped, cacheLines, cacheLineSize, cacheLatency, memoryLatency);
    cache_inst.clk(clk);
    testbench testbench_inst("testbench_inst", numRequests, requests);
    testbench_inst.clk(clk);

    // Signals to pass requests to cache
    // - Address of the next request
    sc_signal<sc_uint<32>> addr_signal;
    testbench_inst.addr(addr_signal);
    cache_inst.addr(addr_signal);

    // - Value of the next request
    sc_signal<sc_uint<32>> data_signal;
    testbench_inst.wdata(data_signal);
    cache_inst.wdata(data_signal);

    // - Write or read request
    sc_signal<bool> we_signal;
    testbench_inst.we(we_signal);
    cache_inst.we(we_signal);

    // Start the simulation with the given number of cycles
    // TODO: Use cycles property to simulate only the specific number of cycles
    sc_start(7, SC_NS);

    // Store the simulation results after the simulation has finished
    simulationResult.cycles = cache_inst.cycles;
    simulationResult.misses = cache_inst.misses;
    simulationResult.hits = cache_inst.hits;

    return 0;
}