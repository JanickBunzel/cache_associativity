#include "cpu.hpp"

void cpu::nextRequest() {
    for(int i = 0; i < numRequests; i++) {
        addr.write(requests[i].addr);
        wdata.write(requests[i].data);
        we.write(requests[i].we);
    }
}

// cpu::cpu(sc_module_name name, int _cycles, int _numRequests, Request* _requests)
//     : sc_module(name), cycles(_cycles), numRequests(_numRequests), requests(_requests) {
cpu::cpu(sc_module_name name, int _numRequests, Request* _requests)
    : sc_module(name), numRequests(_numRequests), requests(_requests) {
    SC_THREAD(nextRequest);
    dont_initialize();
    sensitive << clk.pos();
}