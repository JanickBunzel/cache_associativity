#include <iostream>
#include <systemc.h>
#include "requestStruct.hpp"
// Deklarieren und initialisieren Sie die globale Variable
Request* requestsGlobal;

extern "C" void run_simulation(int numRequests, struct Request requests[]) {
    std::cout << "Hallo von der C++ simulation!" << std::endl;

    requestsGlobal = requests;

    sc_main(numRequests, NULL);

}
