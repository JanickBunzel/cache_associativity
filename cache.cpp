// cache.cpp
#include "cache.hpp"

void cache::cache_access() {
    while (true) {
        wait(SC_ZERO_TIME);
        sc_uint<32> address = addr.read();
        sc_uint<OFFSET_BITS> offset = address.range(OFFSET_BITS - 1, 0);
        sc_uint<INDEX_BITS> index = address.range(OFFSET_BITS + INDEX_BITS - 1, OFFSET_BITS);
        sc_uint<TAG_BITS> tag = address.range(31, OFFSET_BITS + INDEX_BITS);

        if (we.read() == 0) {
            if (cache_storage[index] == tag) {
                cache_hit_count++;
                std::cout << "Cache hit" << std::endl;
            }
            else {
                cache_miss_count++;
                std::cout << "Cache miss" << std::endl;
                cache_storage[index] = tag;
            } 
        }
        else {
            cache_write_count++;
            std::cout << "Cache write" << std::endl;
            cache_storage[index] = tag;
        }
        wait();
    }
}

cache::cache(sc_module_name name) : sc_module(name) {
    cache_access_count = 0;
    cache_miss_count = 0;
    cache_hit_count = 0;
    cache_write_count = 0;

    SC_THREAD(cache_access);
    dont_initialize();  
    sensitive << clk.pos();
}