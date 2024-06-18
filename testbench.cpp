#include "testbench.hpp"

void testbench::process() {
    for(int i = 0; i < 5; i++) {
        addr.write(requests[i].addr);
        wdata.write(requests[i].data);
        we.write(requests[i].we);
        wait();
    }
}

testbench::testbench(sc_module_name name) : sc_module(name) {
    requests[0] = {12345, 0x5678, 1};
    requests[1] = {23456, 0xdef0, 0};
    requests[2] = {23456, 0xdef0, 0};
    requests[3] = {45678, 0x4444, 0};
    requests[4] = {56789, 0x6666, 1};

    SC_THREAD(process);
    dont_initialize();
    sensitive << clk.pos();
}