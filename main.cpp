#include <systemc.h>
#include "Request.h"
#include "Result.h"
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

int sc_main(int argc, char *argv[]) {
    // TODO: Implement Tracefile

    sc_clock clk("clk", 1, SC_NS);

    // Initialize and connect cache and cpu to the clock
    Cache cache_inst("cache_inst", directMapped, cacheLines, cacheLineSize, cacheLatency, memoryLatency);
    cache_inst.clk(clk);
    cpu cpu_inst("cpu_inst", numRequests, requests);
    cpu_inst.clk(clk);


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

    
    // Start the simulation with the given number of cycles
    // TODO: Use cycles property to simulate only the specific number of cycles
    sc_start(7, SC_NS);

    // TODO: Return the simulation result

    return 0;
}