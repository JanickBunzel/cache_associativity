#ifndef TESTBENCH_HPP
#define TESTBENCH_HPP
#include <systemc.h>
#include "requestStruct.hpp"

SC_MODULE(testbench) {
    sc_in<bool> clk;
    sc_out<sc_uint<32>> addr;
    sc_out<sc_uint<32>> wdata;
    sc_out<bool> we;

    Request requests[5];

    void process();

    SC_CTOR(testbench);
};
#endif