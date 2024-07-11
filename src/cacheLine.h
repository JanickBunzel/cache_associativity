#ifndef CACHELINE_H
#define CACHELINE_H
#include <systemc.h>
#include <vector>

class CacheLine {
public:

    std::vector<sc_uint<8>> data;
    sc_uint<32> tag = 0;
    int lru = 0;
    bool valid = false;
    unsigned cacheLineSize;

    CacheLine(unsigned cacheLineSize);

    void write(sc_uint<32> newTag, const std::vector<sc_uint<8>>& newData);
    bool read(sc_uint<32> readTag, std::vector<sc_uint<8>>& outData);
    void updateLru(int newLru);
};

#endif
