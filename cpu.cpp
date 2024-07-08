#include "cpu.hpp"

void Cpu::handleRequests()
{
    int debugPrints = 0;

    for (int i = 0; i < numRequests; i++)
    {
        // Wait for the cache to be done processing
        while (!cacheDone.read())
        {
            if (debugPrints) std::cout << "[Cpu]: " << "Warten auf Cache mit anlegen von Request: " << i+1 << std::endl;
            wait();
        }

        // Pass the next request to the cache
        if (debugPrints) std::cout << "[Cpu]: " << "Nächste Request anlegen: i: " << i << std::endl;
        addr.write(requests[i].addr);
        wdata.write(requests[i].data);
        we.write(requests[i].we);

        wait();
    }

    cpuDone.write(true);
}

Cpu::Cpu(sc_module_name name, int _numRequests, Request *_requests)
    : sc_module(name), numRequests(_numRequests), requests(_requests)
{
    SC_THREAD(handleRequests);
    dont_initialize();
    sensitive << clk.pos();
}