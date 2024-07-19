#ifndef CACHELINE_H
#define CACHELINE_H
#include <systemc.h>
#include <vector>

class CacheLine
{
public:
    CacheLine(unsigned const cachelineSize) : cachelineSize(cachelineSize) { data = std::vector<sc_uint<8>>(cachelineSize); }

    void setData(std::vector<sc_uint<8>> data) { this->data = data; }
    std::vector<sc_uint<8>> getData() { return data; }

    void setTag(sc_uint<32> tag) { this->tag = tag; }
    sc_uint<32> getTag() { return tag; }

    void setValid(bool valid) { this->valid = valid; }
    bool getValid() { return valid; }

    void setLru(int lru) { this->lru = lru; }
    int getLru() { return lru; }

private:
    // Data stored in the cache line as a vector of bytes
    std::vector<sc_uint<8>> data;
    sc_uint<32> tag = 0;

    // Least Recently Used counter
    int lru = 0;

    // Flag to indicate if the cache line is valid
    bool valid = false;
    
    const unsigned cachelineSize;
};

#endif
