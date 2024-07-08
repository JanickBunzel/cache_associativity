#include "directMappedCache.h"

void DirectMappedCache::cacheAccess()
{
    while (true)
    {
        cacheDone.write(false);
        this->statistics.accesses++;
        wait(SC_ZERO_TIME);

        if (offsetBits < 0 || indexBits < 0 || offsetBits + indexBits > 31) {
            std::cerr << "Error: offsetBits and/or indexBits are out of valid range." << std::endl;
            break;
        }

        sc_uint<32> address = this->address.read();
        sc_uint<32> offset = 0, index = 0, tag;

        if (offsetBits == 0 && indexBits == 0) {
            tag = address;
        } else if (offsetBits == 0) {
            index = address.range(indexBits - 1, 0);
            tag = address.range(31, indexBits);
        } else if (indexBits == 0) {
            offset = address.range(offsetBits - 1, 0);
            tag = address.range(31, offsetBits);
        } else {
            offset = address.range(offsetBits - 1, 0);
            index = address.range(offsetBits + indexBits - 1, offsetBits);
            tag = address.range(31, offsetBits + indexBits);
        }

        if (we.read() == 0)
        {
            this->statistics.reads++;
            sc_uint<32> rdata;
            for (unsigned i = 0; i < cacheLatency; ++i) {
                wait();
            }
            const bool hit = cacheLines[index].readDirect(tag, rdata);
            if (hit)
            {
                this->statistics.hits++;
            }
            else
            {
                this->statistics.misses++;

            }
        }

    }
}