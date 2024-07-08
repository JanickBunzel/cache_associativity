#include "cpu.hpp"

void Cpu::handleRequests()
{
    for (int i = 0; i < numRequests; i++)
    {
        // Wait for the cache to be done processing
        while (!cacheDone.read())
        {
            std::cout << "[Cpu]: : " << "warten bis cache fertig ist: i:" << i << std::endl;
            wait();
        }

        std::cout << "[Cpu]: " << "Nächste Adresse: i: " << i << std::endl;
        // Pass the next request to the cache
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
    sensitive << clk.pos();
}