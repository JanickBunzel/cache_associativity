#include "testbench.hpp"

void testbench::process() {
    for(int i = 0; i < numRequests; i++) {
        addr.write(requests[i].addr);
        wdata.write(requests[i].data);
        we.write(requests[i].we);
        wait();
    }
}

testbench::testbench(sc_module_name name, int numReqs, Request* reqs) 
    : sc_module(name), requests(reqs), numRequests(numReqs) {
    SC_THREAD(process);
    dont_initialize();
    sensitive << clk.pos();
}