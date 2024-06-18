#include <iostream>
#include <systemc.h>

extern "C" void run_simulation(int numRequests, struct Request requests[]) {
    std::cout << "Hallo von der C++ simulation!" << std::endl;

    char *argv = reinterpret_cast<char*>(requests);

    sc_main(numRequests, &argv);
}
