#include <iostream>
#include <iomanip>
#include "cpu.hpp"

// Global variables provided by the rahmenprogramm (cache_simulaton option), specifies how the debug information is printed
extern int printsLevel;
extern char *highlightColor;
extern char *resetColor;

Cpu::Cpu(sc_module_name name, Request *requests, const int requestLength)
    : sc_module(name), cpuStatistics({0, 0, 0}), requests(requests), requestLength(requestLength)
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
            if (printsLevel >= 3)
            {
                std::cout << "Cycles[" << cycles++ << "]" << std::endl;
            }

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
                cpuStatistics.allRequestsProcessed = true;

                // Handle the result of a read request (write result back and print)
                if (!requests[i].we)
                {
                    requests[i].data = cacheReadDataCPUIn.read();
                }
                printResultOfRequest(i);
                sc_stop();
                return;
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
    return;
}

void Cpu::printProccessingOfRequest(unsigned requestIndex)
{
    if (printsLevel == 0)
    {
        return;
    }

    std::cout << highlightColor; // Start highlight color
    std::cout << "[Cpu]: request[" << requestIndex << "]: "
        << (requests[requestIndex].we ? "W," : "R,")
        << std::hex << "0x" << requests[requestIndex].addr;
    if (requests[requestIndex].we)
    {
        std::cout << ", 0x" << requests[requestIndex].data;
    }
    std::cout << "\n" << std::dec << resetColor << std::endl; // Reset color back to white
}

void Cpu::printResultOfRequest(unsigned requestIndex)
{
    if (printsLevel == 0)
    {
        return;
    }

    std::cout << highlightColor; // Start highlight color

    std::cout << "[Cpu]: Cache done processing request[" << requestIndex << "].";
    if (!requests[requestIndex].we)
    {
        std::cout << " (Read request, data received: 0x" << std::setw(8) << std::setfill('0') << std::hex << (0xFFFFFFFF & cacheReadDataCPUIn.read()) << std::dec << ")" << std::endl;
    }
    std::cout << std::endl;

    std::cout << resetColor << std::endl; // Reset color back to white
}
