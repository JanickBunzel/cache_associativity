#ifndef CPU_HPP
#define CPU_HPP

#include <systemc.h>
#include "request.h"

typedef struct CpuStatistics
{
    unsigned cycles;
    unsigned requests;
    bool allRequestsProcessed;
} CpuStatistics;

SC_MODULE(Cpu)
{
    // --- INPUT PORTS --- //

    sc_in<bool> clkCPUIn;
    // Signal to indicate that the cache has finished processing the request
    sc_in<bool> cacheDoneCPUIn;
    // Signal that contains the data read from the cache (One Byte)
    sc_in<sc_uint<32>> cacheReadDataCPUIn;

    // --- OUTPUT PORTS --- //

    // Signal containing the address of the request
    sc_out<sc_uint<32>> addressCPUOut;
    // Signal containing the data of the request, if it is a write request
    sc_out<sc_uint<32>> writeDataCPUOut;
    // Signal indicating whether the request is a write request
    sc_out<bool> writeEnableCPUOut;

    // --- INTERNAL VARIABLES --- //

    CpuStatistics cpuStatistics;
    Request *requests;
    int requestLength;

    void handleRequests();
    void printProccessingOfRequest(unsigned requestIndex);
    void printResultOfRequest(unsigned requestIndex);

    SC_CTOR(Cpu);

    Cpu(sc_module_name name, Request *requests, int requestLength);
};

#endif