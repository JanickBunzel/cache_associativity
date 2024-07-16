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
        while (cacheReadDataCPUIn.read() == 0)
        {
            cpuStatistics.cycles++;
            std::cout << "Cycle[" << cycles++ << "]" << std::endl;

            wait(SC_ZERO_TIME);
            wait(SC_ZERO_TIME);
            wait(SC_ZERO_TIME);
            wait(SC_ZERO_TIME);
            wait(SC_ZERO_TIME);

            if (cacheDoneCPUIn.read() == true)
            {
                std::cout << "cpu detected that cache is done" << std::endl;
                break;
            }

            wait();
        }
        std::cout << "" << std::endl;
        std::cout << "###########################################################################################################################################################################" << std::endl;
        std::cout << "[Cpu]: Cache done processing request[" << i << "]" << std::endl;
        std::cout << "[Cpu]: Read Data: " << cacheReadDataCPUIn.read() << std::endl;
        std::cout << "###########################################################################################################################################################################" << std::endl;

        if ( i == requestLength - 1) { break; }

        wait();
    }

    sc_stop();
}
