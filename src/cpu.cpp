#include "cpu.hpp"
#include <iostream>

Cpu::Cpu(sc_module_name name, Request *requests, const int requestLength)
    : sc_module(name), cpuStatistics({0, 0}), requests(requests), requestLength(requestLength)
{
    SC_THREAD(handleRequests);
    dont_initialize();
    sensitive << clkCPUIn.pos();
}

void Cpu::handleRequests()
{
    unsigned cycles = 1;

    for (int i = 0; i < requestLength; i++)
    {
        cpuStatistics.requests++;
        std::cout << "" << std::endl;
        std::cout << "###########################################################################################################################################################################" << std::endl;
        std::cout << "[Cpu]: Request[" << i << "]: " << (requests[i].we ? "W," : "R,") << requests[i].addr << "," << (requests[i].we ? std::to_string(requests[i].data) : "") << std::endl;
        std::cout << "###########################################################################################################################################################################" << std::endl;

        // Send the request to the cache
        addressCPUOut.write(requests[i].addr);
        writeDataCPUOut.write(requests[i].data);
        writeEnableCPUOut.write(requests[i].we);

        // Wait for the cache to be done processing
        while (cacheDone == false)
        {
            cpuStatistics.cycles++;
            std::cout << "Cycles[" << cycles++ << "]" << std::endl;

            // Wait for the Cpu-Cache signals
            wait(SC_ZERO_TIME);
            // Wait for the Cache-Memory signals
            wait(SC_ZERO_TIME);
            // Wait for the Memory-Cache signals
            wait(SC_ZERO_TIME);
            // Wait for the Cache-Cpu signals
            wait(SC_ZERO_TIME);

            if (cacheDoneCPUIn.read() == true && i == requestLength - 1)
            {
                sc_stop();
            }

            wait();
            cacheDone = cacheDoneCPUIn.read();
        }
        std::cout << "" << std::endl;
        std::cout << "###########################################################################################################################################################################" << std::endl;
        std::cout << "[Cpu]: Cache done processing request[" << i << "]" << std::endl;
        std::cout << std::hex << "[Cpu]: Read Data: " << cacheReadDataCPUIn.read() << std::dec << std::endl;
        std::cout << "###########################################################################################################################################################################" << std::endl;

        cacheDone = false;
    }

    sc_stop();
}
