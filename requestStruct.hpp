#ifndef REQUESTSTRUCT_HPP
#define REQUESTSTRUCT_HPP
#include <cstdint>

struct Request { 
    uint32_t addr; 
    uint32_t data; 
    int we;
};

#endif