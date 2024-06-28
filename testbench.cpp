#include "testbench.hpp"

void testbench::nextRequest()
{
    for (int i = 0; i < numRequests; i++)
    {
        addr.write(requests[i].addr);
        wdata.write(requests[i].data);
        we.write(requests[i].we);

        wait();
        /* TODO: Wait for the cache to be done processing
        while (!done)
        {
            wait();
        }
        */
    }
}

testbench::testbench(sc_module_name name, int _numRequests, Request *_requests)
    : sc_module(name), numRequests(_numRequests), requests(_requests)
{
    SC_THREAD(nextRequest);
    dont_initialize();
    sensitive << clk.pos();
}