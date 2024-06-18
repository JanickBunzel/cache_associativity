#include <systemc.h>
#include "requestStruct.hpp"
#include "cache.hpp"
#include "testbench.hpp"
#include <sstream>

int sc_main(int argc, char *argv[])
{

    int numRequests = argc;

    // Allocate memory for the requests array
    Request* requests = new Request[numRequests];
    
    // Parse the request data from the arguments
    for (int i = 0; i < numRequests; ++i) {
        // Convert the argument to a string
        std::string arg = argv[i + 1];

        // Split the string into tokens
        std::istringstream iss(arg);
        std::vector<std::string> tokens(std::istream_iterator<std::string>{iss}, std::istream_iterator<std::string>());

        // Assign the tokens to the request fields
        requests[i].addr = std::stoi(tokens[0]);
        requests[i].data = std::stoi(tokens[1]);
        requests[i].we = std::stoi(tokens[2]) != 0;
    }

    sc_signal<bool> we;
    sc_clock clk("clk", 1, SC_NS);

    cache cache_inst("cache_inst");
    cache_inst.clk(clk);

    testbench testbench_inst("testbench_inst");
    testbench_inst.clk(clk);

    sc_signal<sc_uint<32>> addr_tb;
    sc_signal<sc_uint<32>> wdata_tb;
    sc_signal<bool> we_tb;

    testbench_inst.addr(addr_tb);
    cache_inst.addr(addr_tb);

    testbench_inst.wdata(wdata_tb);
    cache_inst.wdata(wdata_tb);
    
    testbench_inst.we(we_tb);
    cache_inst.we(we_tb);

    sc_start(5, SC_NS);

    delete[] requests;

    return 0;
}