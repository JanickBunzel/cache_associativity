#ifndef MEMORY_H
#define MEMORY_H
#include <systemc.h>
#include <unordered_map>
#include <vector>

SC_MODULE(Memory)
{
    // --- INPUT PORTS --- //

    sc_in<bool> clkMEMORYIn;
    // Signal containing the address of the request
    sc_in<sc_uint<32>> addressMEMORYIn;
    // Signal containing the data of the request, if it is a write request
    sc_in<sc_uint<32>> writeDataMEMORYIn;
    // Signal indicating whether the request is a write request
    sc_in<bool> writeEnableMEMORYIn;
    // Signal to indicate that the memory can start processing the request
    sc_in<bool> enableMEMORYIn;

    // --- OUTPUT PORTS --- //

    // Signal containing the data read from the memory (full cache line)
    std::vector<sc_out<sc_uint<8>>> readDataMEMORYOut;
    // Signal to indicate that the memory has finished processing the request
    sc_out<bool> doneMEMORYOut;

    // --- INTERNAL VARIABLES --- //

    std::unordered_map<unsigned, sc_uint<8>> memory;
    unsigned memoryLatency;
    unsigned cachelineSize;

    void memoryAccess();
    void write(unsigned memoryAddress, sc_uint<32> data);
    std::vector<sc_uint<8>> readBlock(unsigned address);
    void printMemory();

    SC_CTOR(Memory);

    Memory(sc_module_name name, unsigned memoryLatency, unsigned cachelineSize);
};

#endif
