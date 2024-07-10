#include <systemc.h>
#include "request.h"
#include "result.h"
#include "cpu.hpp"
#include "directMappedCache.h"
#include "memory.h"

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

    // ----- Cache ----- //
    DirectMappedCache cache("cache_inst", cacheLines, cacheLatency, cacheLineSize);
    cache.clkCACHEIn(clk);

    // ----- CPU ----- //
    Cpu cpu("cpu_inst", requests, numRequests);
    cpu.clkCPUIn(clk);

    // ----- Memory ----- //
    Memory memory("memory_inst", memoryLatency, cacheLineSize);
    memory.clkMEMORYIn(clk);

    // ----- Signals Cache ----- //

    // - Address of the next request
    sc_signal<sc_uint<32>> cacheAdressSignal;
    cpu.addressCPUOut(cacheAdressSignal);
    cache.cacheAddressCACHEIn(cacheAdressSignal);
    // - Value of the next request
    sc_signal<sc_uint<32>> cacheWriteDataSignal;
    cpu.writeDataCPUOut(cacheWriteDataSignal);
    cache.cacheWriteDataCACHEIn(cacheWriteDataSignal);
    // - Write or read request
    sc_signal<bool> cacheWriteEnableSignal;
    cpu.writeEnableCPUOut(cacheWriteEnableSignal);
    cache.cacheWriteEnableCACHEIn(cacheWriteEnableSignal);
    // - Cache read data
    sc_signal<sc_uint<8>> cacheReadDataSignal;
    cpu.cacheReadDateCPUIn(cacheReadDataSignal);
    cache.cacheReadDataCACHEOut(cacheReadDataSignal);
    // - Cache done signal
    sc_signal<bool> cacheDoneSignal;
    cpu.cacheDoneCPUIn(cacheDoneSignal);
    cache.cacheDoneCACHEOut(cacheDoneSignal);
    // ----- Signals Memory ----- //

    // - Address of the memory request
    sc_signal<sc_uint<32>> memoryAddressSignal;
    cache.memoryAddressCACHEOut(memoryAddressSignal);
    memory.addressMEMORYIn(memoryAddressSignal);
    // - Value of the memory write request
    sc_signal<sc_uint<32>> memoryWriteDataSignal;
    cache.memoryWriteDataCACHEOut(memoryWriteDataSignal);
    memory.writeDataMEMORYIn(memoryWriteDataSignal);
    // - Write memory signal
    sc_signal<bool> memoryWriteEnableSignal;
    cache.memoryWriteEnableCACHEOut(memoryWriteEnableSignal);
    memory.writeEnableMEMORYIn(memoryWriteEnableSignal);
    // - Memory enable signal
    sc_signal<bool> memoryEnableSignal;
    cache.memoryEnableCACHEOut(memoryEnableSignal);
    memory.enableMEMORYIn(memoryEnableSignal);
    // - Memory read data
    std::vector<sc_signal<sc_uint<8>>*> memoryReadDataSignals(cacheLineSize, nullptr);
    for (size_t i = 0; i < cacheLineSize; i++) {
        memoryReadDataSignals[i] = new sc_signal<sc_uint<8>>();
    }
    for (size_t i = 0; i < cacheLineSize; i++) {
        auto signal = memoryReadDataSignals[i];
        cache.memoryReadDataCACHEIn[i](*signal);
    }
    for (size_t i = 0; i < cacheLineSize; i++) {
        auto signal = memoryReadDataSignals[i];
        memory.readDataMEMORYOut[i](*signal);
    }
    // - Memory done signal
    sc_signal<bool> memoryDoneSignal;
    cache.memoryDoneCACHEIn(memoryDoneSignal);
    memory.doneMEMORYOut(memoryDoneSignal);

    // Create a tracefile to track the simulation signals
    //sc_trace_file *tf;
    //if (strcmp(tracefile, "") != 0) {
    //    tf = sc_create_vcd_trace_file(tracefile);
    //    sc_trace(tf, cache_done_signal, "cache_done_signal");
    //    sc_trace(tf, cache_addr_signal, "addr_signal");
    //    sc_trace(tf, cache_we_signal, "we_signal");
    //}

    // Start the simulation with the given number of cycles
    sc_start(cycles, SC_NS);

    // Close the tracefile
    //if (strcmp(tracefile, "") != 0) {
    //    sc_close_vcd_trace_file(tf);
    //}

    // Store the simulation results after the simulation has finished
    simulationResult.cycles = cpu.cpuStatistics.cycles;
    simulationResult.misses = cache.statistics.misses;
    simulationResult.hits = cache.statistics.hits;
    simulationResult.primitiveGateCount = primitiveGateCount();

    // Clean up dynamically allocated memory
    for (auto signal : memoryReadDataSignals) {
        delete signal;
    }

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