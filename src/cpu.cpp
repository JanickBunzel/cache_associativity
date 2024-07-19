#include "cpu.hpp"
#include <iostream>

// Flag passed from the rahmenprogramm (cache_simulaton parameter)
extern int printsEnabled;

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
    bool cacheDone = false;

    for (int i = 0; i < requestLength; i++)
    {
        cpuStatistics.requests++;
        printProccessingOfRequest(i);

        // Send the request to the cache
        addressCPUOut.write(requests[i].addr);
        writeDataCPUOut.write(requests[i].data);
        writeEnableCPUOut.write(requests[i].we);

        // Wait for the cache to be done processing
        while (cacheDone == false)
        {
            cpuStatistics.cycles++;
            if (printsEnabled) std::cout << "Cycles[" << cycles++ << "]" << std::endl;

            // Wait for the Cpu-Cache signals
            wait(SC_ZERO_TIME);
            // Wait for the Cache-Memory signals
            wait(SC_ZERO_TIME);
            // Wait for the Memory-Cache signals
            wait(SC_ZERO_TIME);
            // Wait for the Cache-Cpu signals
            wait(SC_ZERO_TIME);

            // If all requests have been processed, don't wait another cycle and stop the simulation
            if (cacheDoneCPUIn.read() == true && i == requestLength - 1)
            {
                // Handle the result of a read request (write result back and print)
                if (!requests[i].we)
                {
                    requests[i].data = cacheReadDataCPUIn.read();
                }
                printResultOfRequest(i);
                sc_stop();
            }

            wait();
            cacheDone = cacheDoneCPUIn.read();
        }
        // Handle the result of a read request (write result back and print)
        if (!requests[i].we)
        {
            requests[i].data = cacheReadDataCPUIn.read();
        }
        printResultOfRequest(i);

        // Reset the cacheDone flag
        cacheDone = false;
    }

    sc_stop();
}

void Cpu::printProccessingOfRequest(unsigned requestIndex)
{
    if (!printsEnabled)
    {
        return;
    }

    std::cout << "\033[0;36m" << std::endl;
    std::cout << "###########################################################################################################################################################################" << std::endl;
    std::cout << "[Cpu]: Processing... Request[" << requestIndex << "]: " << (requests[requestIndex].we ? "W," : "R,") << std::hex << "0x" << requests[requestIndex].addr << std::dec << "," << (requests[requestIndex].we ? std::to_string(requests[requestIndex].data) : "") << std::endl;
    std::cout << "###########################################################################################################################################################################";
    std::cout << "\033[0m" << std::endl;
}

void Cpu::printResultOfRequest(unsigned requestIndex)
{
    if (!printsEnabled)
    {
        return;
    }
    
    std::cout << "\033[0;36m";
    std::cout << "###########################################################################################################################################################################" << std::endl;
    std::cout << "[Cpu]: Cache done processing request[" << requestIndex << "]" << std::endl;
    std::cout << std::hex << "[Cpu]: Read Data: 0x" << cacheReadDataCPUIn.read() << std::dec << std::endl;
    std::cout << "###########################################################################################################################################################################";
    std::cout << "\033[0m" << std::endl;
}