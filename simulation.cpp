#include <iostream>
#include <systemc.h>
#include "Request.h"

// Globale Variable für die Simulation
Request* requestsGlobal;

extern "C" void run_simulation(int numRequests, struct Request requests[]) {
    std::cout << "Hallo von der C++ simulation!" << std::endl;

    requestsGlobal = requests;

    sc_main(numRequests, NULL);

}
