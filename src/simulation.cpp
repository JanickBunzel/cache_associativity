#include <systemc.h>
#include "request.h"
#include "result.h"
#include "cpu.hpp"
#include <sstream>

int cycles;
int directMapped;
unsigned cachelines;
unsigned cachelineSize;
unsigned cacheLatency;
unsigned memoryLatency;
size_t numRequests;
Request *requests;
const char *tracefile;

extern Result simulationResult;

extern "C" Result run_simulation(
    int _cycles,
    int _directMapped,
    unsigned _cachelines,
    unsigned _cachelineSize,
    unsigned _cacheLatency,
    unsigned _memoryLatency,
    size_t _numRequests,
    struct Request _requests[],
    const char *_tracefile)
{
    cycles = _cycles;
    directMapped = _directMapped;
    cachelines = _cachelines;
    cachelineSize = _cachelineSize;
    cacheLatency = _cacheLatency;
    memoryLatency = _memoryLatency;
    numRequests = _numRequests;
    requests = _requests;
    tracefile = _tracefile;

    // Call the SystemC Simulation in main.cpp
    if (numRequests <= 0 || sc_main(0, nullptr) != 0)
    {
        // On error, return an empty result
        Result errorResult = {
            .cycles = 0,
            .misses = 0,
            .hits = 0,
            .primitiveGateCount = 0};
        return errorResult;
    }
    else
    {
        // If no error, return the simulation result
        return simulationResult;
    }
}