#ifndef MEMORY_H
#define MEMORY_H
#include <systemc.h>
#include <unordered_map>

SC_MODULE(Memory)
{
    SC_CTOR(Memory);

    std::unordered_map<unsigned, sc_uint<8>> memory;
    unsigned memoryLatency;

    sc_in<sc_uint<32>> address;
    sc_in<sc_uint<32>> wdata;
    sc_out<sc_uint<32>> rdata;
    sc_out<bool> memoryDone;
    sc_in<bool> clock;
    sc_in<bool> we;

    Memory(sc_module_name name, unsigned memoryLatency)
    : sc_module(name), memoryLatency(memoryLatency)
    {
        SC_THREAD(memoryAccess);
        sensitive << clock.pos();
    }

    void memoryAccess();
    void write(unsigned addr, sc_uint<32> data);
    sc_uint<32> read(unsigned addr);
};

#endif

