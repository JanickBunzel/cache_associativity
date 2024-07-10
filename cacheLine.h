#ifndef CACHELINE_H
#define CACHELINE_H
#include <systemc.h>
#include <vector>

class CacheLine {
public:

    std::vector<sc_uint<8>> data;
    sc_uint<32> tag{0};
    sc_int<2> lru{0};
    bool valid {false};
    unsigned cacheLineSize;

    CacheLine(unsigned cacheLineSize);

    void write(sc_uint<32> newTag, const std::vector<sc_uint<8>>& newData, sc_uint<2> newLru =0);
    bool read(sc_uint<32> readTag, std::vector<sc_uint<8>>& outData, sc_uint<2> newLru =0);
    void updateLru(sc_uint<2> newLru);
};

#endif
