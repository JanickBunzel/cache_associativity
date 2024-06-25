#include <systemc.h>
#include "Request.h"
#include "Result.h"
#include "cache.hpp"
#include "testbench.hpp"
#include <sstream>

int cycles;
int directMapped;
unsigned cacheLines;
unsigned cacheLineSize;
unsigned cacheLatency;
unsigned memoryLatency;
size_t numRequests;
Request *requests;
const char *tracefile;

extern Result simulationResult;

extern "C" struct Result run_simulation(
    int _cycles,
    int _directMapped,
    unsigned _cacheLines,
    unsigned _cacheLineSize,
    unsigned _cacheLatency,
    unsigned _memoryLatency,
    size_t _numRequests,
    struct Request _requests[],
    const char *_tracefile)
{
    cycles = _cycles;
    directMapped = _directMapped;
    cacheLines = _cacheLines;
    cacheLineSize = _cacheLineSize;
    cacheLatency = _cacheLatency;
    memoryLatency = _memoryLatency;
    numRequests = _numRequests;
    requests = _requests;
    tracefile = _tracefile;

    if (sc_main(0, nullptr) == 0) {
        // Kein Fehler in der Simulation
        return simulationResult;
    } else {
        // Return invalides Result
        Result errorResult = {
            .cycles = 0,
            .misses = 0,
            .hits = 0,
            .primitiveGateCount = 0
        };
        return errorResult;
    }
}