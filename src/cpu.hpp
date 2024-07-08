#ifndef CPU_HPP
#define CPU_HPP
#include <systemc.h>
#include "request.h"

SC_MODULE(Cpu)
{
    // SystemC Input Ports:
    sc_in<bool> clk;
    sc_in<bool> cacheDone;

    // SystemC Output Ports:
    sc_out<bool> cpuDone;
    sc_out<sc_uint<32>> addr;  // Address of the next request
    sc_out<sc_uint<32>> wdata; // Value of the next request
    sc_out<bool> we;           // Write or read request

    int numRequests;
    Request *requests;

    void handleRequests();

    Cpu(sc_module_name name, int _numRequests, Request *_requests);

    SC_CTOR(Cpu);
};

#endif