#include <systemc.h>
#include "Request.h"
#include "Result.h"
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
    std:cout << "sc_main Method called" << std::endl;
    
    // TODO: Use the config, tracefile etc.

    sc_signal<bool> we;
    sc_clock clk("clk", 1, SC_NS);

    Cache cache_inst(
        "cache_inst",
        directMapped,
        cacheLines,
        cacheLineSize
    );
    cache_inst.clk(clk);

    testbench testbench_inst("testbench_inst", numRequests, requests);
    testbench_inst.clk(clk);

    sc_signal<sc_uint<32>> addr_tb;
    sc_signal<sc_uint<32>> wdata_tb;
    sc_signal<bool> we_tb;

    testbench_inst.addr(addr_tb);
    cache_inst.addr(addr_tb);

    testbench_inst.wdata(wdata_tb);
    cache_inst.wdata(wdata_tb);
    
    testbench_inst.we(we_tb);
    cache_inst.we(we_tb);

    sc_start(7, SC_NS);

    return 0;
}