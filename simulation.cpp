#include <iostream>
#include <systemc.h>

extern "C" void run_simulation() {
    std::cout << "Hallo von der C++ simulation!" << std::endl;
    sc_main(0, NULL);
}
