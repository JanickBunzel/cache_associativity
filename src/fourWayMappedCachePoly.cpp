//
// Created by julian on 10.07.24.
//

#include "fourWayMappedCachePoly.h"
#include "cache.h"
#include <iostream>
#include <cmath>

FourWayMappedCachePoly::FourWayMappedCachePoly(sc_module_name name, unsigned cacheSize, unsigned cacheLatency, unsigned cacheLineSize)
    : Cache(name, cacheSize, cacheLatency, cacheLineSize)
{
    calculateBits(cacheSize, cacheLineSize);

    SC_THREAD(cacheAccess);
    dont_initialize();
    sensitive << clkCACHEIn.pos();

    printBits();
    printCache();
}

void FourWayMappedCachePoly::cacheAccess()
{
    while (true)
    {
        wait(SC_ZERO_TIME);

        cacheDoneCACHEOut.write(false);
        this->statistics.accesses++;

        if (bits.offset < 0 || bits.index < 0 || bits.offset + bits.index > 31) {
            std::cerr << "Error: offsetBits and/or indexBits are out of valid range." << std::endl;
            break;
        }

        sc_uint<32> address = this->cacheAddressCACHEIn.read();
        sc_uint<32> offset = 0, index = 0, tag;

        if (bits.offset == 0 && bits.index == 0) {
            tag = address;
        } else if (bits.offset == 0) {
            index = address.range(bits.index - 1, 0);
            tag = address.range(31, bits.index);
        } else if (bits.index == 0) {
            offset = address.range(bits.offset - 1, 0);
            tag = address.range(31, bits.offset);
        } else {
            offset = address.range(bits.offset - 1, 0);
            index = address.range(bits.offset + bits.index - 1, bits.offset);
            tag = address.range(31, bits.offset + bits.index);
        }

        if (cacheWriteEnableCACHEIn.read() == 0)
        {
           //TODO: Implement read access
        }
        else if (cacheWriteEnableCACHEIn.read() == 1)
        {
           //TODO: Implement write access
        }
        memoryEnableCACHEOut.write(false);
        cacheDoneCACHEOut.write(true);
        printCache();
        wait();
    }
}

void FourWayMappedCachePoly::printCache()
{
    std::cout << "Cache State:" << std::endl;
    std::cout << "-----------------------------------------------------" << std::endl;
    std::cout << "Set\tWay\tTag\t\tValid\tLRU\tData (Hex/Binary)" << std::endl;
    std::cout << "-----------------------------------------------------" << std::endl;

    for (unsigned i = 0; i < cacheLines.size(); i += 4) {
        unsigned set = i / 4; // Berechnet die Set-Nummer
        for (unsigned way = 0; way < 4; ++way) {
            const auto& line = cacheLines[i + way];
            std::cout << set << "\t" << way << "\t"
                      << line.tag.to_string(SC_HEX) << "\t"
                      << line.valid << "\t"
                      << line.lru << "\t";
            for (const auto& byte : line.data) {
                std::cout << "("
                          << byte.to_string(SC_HEX) << " / "
                          << byte.to_string(SC_BIN) << ") ";
            }
            std::cout << std::endl;
        }
        std::cout << "-----------------------------------------------------" << std::endl;
    }

    std::cout << "Statistics:" << std::endl;
    std::cout << "Hits: " << statistics.hits << std::endl;
    std::cout << "Misses: " << statistics.misses << std::endl;
    std::cout << "Accesses: " << statistics.accesses << std::endl;
    std::cout << "Writes: " << statistics.writes << std::endl;
    std::cout << "Reads: " << statistics.reads << std::endl;
    std::cout << "-----------------------------------------------------" << std::endl;
}

void FourWayMappedCachePoly::calculateBits(unsigned cacheSize, unsigned cacheLineSize)
{
    unsigned numberOfSets = cacheSize / (4 * cacheLineSize);

    bits.offset = static_cast<unsigned int>(std::ceil(std::log2(static_cast<double>(cacheLineSize))));
    bits.index = static_cast<unsigned int>(std::ceil(std::log2(static_cast<double>(numberOfSets))));
    bits.tag = 32 - bits.offset - bits.index;
}
