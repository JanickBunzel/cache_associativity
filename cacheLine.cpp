#include "cacheLine.h"

// Default constructor that initializes data with zeros
CacheLine::CacheLine(unsigned cacheLineSize) : data(cacheLineSize), cacheLineSize(cacheLineSize)  {}

// Function to write new data into the cache line
void CacheLine::write(sc_uint<32> newTag, const std::vector<sc_uint<8>>& newData, sc_uint<2> newLru) {
    tag = newTag;
    data = newData;
    lru = newLru;
    valid = true;
}

// Function to read data from the cache line
bool CacheLine::read(sc_uint<32> readTag, std::vector<sc_uint<8>>& outData, sc_uint<2> newLru) {
    if (tag == readTag && valid) {
        outData = data;
        lru = newLru;
        return true;
    } else {
        return false;
    }
}

// Function to update the LRU value
void CacheLine::updateLru(sc_uint<2> newLru) {
    lru = newLru;
}
