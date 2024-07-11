#include "cacheLine.h"

// Default constructor that initializes data with zeros
CacheLine::CacheLine(unsigned cacheLineSize) : data(cacheLineSize), cacheLineSize(cacheLineSize)  {}

// Function to write new data into the cache line
void CacheLine::write(sc_uint<32> newTag, const std::vector<sc_uint<8>>& newData) {
    tag = newTag;
    data = newData;
    valid = true;
}

// Function to read data from the cache line
bool CacheLine::read(sc_uint<32> readTag, std::vector<sc_uint<8>>& outData) {
    if (tag == readTag && valid) {
        outData = data;
        return true;
    } else {
        return false;
    }
}

// Function to update the LRU value
void CacheLine::updateLru(int newLru) {
    lru = newLru;
}
