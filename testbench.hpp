#ifndef TESTBENCH_HPP
#define TESTBENCH_HPP
#include <systemc.h>
#include "Request.h"

SC_MODULE(testbench) {
    sc_in<bool> clk;
    sc_out<sc_uint<32>> addr;
    sc_out<sc_uint<32>> wdata;
    sc_out<bool> we;

    Request* requests;
    int numRequests;

    void process();

    testbench(sc_module_name name, int numReqs, Request* reqs);  

    SC_CTOR(testbench);
};
#endif