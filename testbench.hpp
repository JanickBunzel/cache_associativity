#ifndef TESTBENCH_HPP
#define TESTBENCH_HPP
#include <systemc.h>
#include "request.h"

SC_MODULE(testbench)
{
    sc_in<bool> clk;
    sc_out<sc_uint<32>> addr;
    sc_out<sc_uint<32>> wdata;
    sc_out<bool> we;

    // int cycles;
    int numRequests;
    Request *requests;

    void nextRequest();

    testbench(sc_module_name name, int _numRequests, Request *_requests);

    SC_CTOR(testbench);
};

#endif