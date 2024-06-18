#ifndef CACHE_HPP
#define CACHE_HPP
#include <systemc.h>

#define CACHE_SIZE 256
#define NUM_BLOCKS (CACHE_SIZE / BLOCK_SIZE)

const int OFFSET_BITS = 6;
const int INDEX_BITS = 8;
const int TAG_BITS = 32 - OFFSET_BITS - INDEX_BITS;

SC_MODULE(cache) {
    sc_in<sc_uint<32>> addr;
    sc_in<sc_uint<32>> wdata;
    sc_in<bool> clk;
    sc_in<bool> we;

    sc_uint<32> cache_storage[CACHE_SIZE];

    int cache_access_count;
    int cache_miss_count;
    int cache_hit_count;
    int cache_write_count;

    void cache_access();

    SC_CTOR(cache);
};
#endif