#ifndef CACHELINE_H
#define CACHELINE_H
#include <systemc.h>

SC_MODULE(CacheLine) {
    sc_uint<32> tag;
    sc_uint<32> data;
    bool valid;
    sc_uint<2> lru;

    CacheLine(sc_module_name name) : tag(0), data(0), valid(false), lru(-1) {}

    void write(sc_uint<32> new_tag, sc_uint<32> new_data, sc_uint<2> new_lru) {
        tag = new_tag;
        data = new_data;
        valid = true;
        lru = new_lru;
    }

    bool read(sc_uint<32> read_tag, sc_uint<32>& out_data, sc_uint<2> new_lru) {
        if (valid && tag == read_tag) {
            out_data = data;
            lru = new_lru;
            return true;
        }
        return false;
    }

    void invalidate() {
        valid = false;
    }

    void update_lru(sc_uint<2> new_lru) {
        lru = new_lru;
    }
};

#endif
