#include "cpu.hpp"
#include <iostream>

Cpu::Cpu(sc_module_name name, Request *requests, const int requestLength)
    : sc_module(name), cpuStatistics({0,0}), requests(requests), requestLength(requestLength)
{
    SC_THREAD(handleRequests);
    dont_initialize();
    sensitive << clkCPUIn.pos();
}

void Cpu::handleRequests()
{
    bool cacheDone = false;
    for (int i = 0; i < requestLength; i++)
    {
        cpuStatistics.requests++;
        std::cout << "[Cpu]: " << "Request mit adresse: " << requests[i].addr << " wurde gestartet! Warte auf Cache..." << std::endl;

        // Send the request to the cache
        addressCPUOut.write(requests[i].addr);
        writeDataCPUOut.write(requests[i].data);
        writeEnableCPUOut.write(requests[i].we);

        // Wait for the cache to be done processing
        while (cacheDone == false)
        {
            cpuStatistics.cycles++;
            std::cout << "[Cpu]: Zyklus: " << cpuStatistics.cycles << std::endl;
            wait();
            cacheDone = cacheDoneCPUIn.read();
        }
        cacheDone = false;
    }

    sc_stop();
}
