#ifndef CPU_HPP
#define CPU_HPP
#include <systemc.h>
#include "request.h"

SC_MODULE(cpu)
{
    sc_in<bool> clk;
    sc_out<sc_uint<32>> addr;
    sc_out<sc_uint<32>> wdata;
    sc_out<bool> we;

    // int cycles;
    int numRequests;
    Request *requests;

    void nextRequest();

    cpu(sc_module_name name, int _numRequests, Request *_requests);

    SC_CTOR(cpu);
};

#endif