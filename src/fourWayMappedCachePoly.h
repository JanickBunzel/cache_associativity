//
// Created by julian on 10.07.24.
//

#ifndef FOURWAYMAPPEDCACHEPOLY_H
#define FOURWAYMAPPEDCACHEPOLY_H

#include "cache.h"

class FourWayMappedCachePoly : public Cache
{
public:
    SC_HAS_PROCESS(FourWayMappedCachePoly);

    FourWayMappedCachePoly(sc_module_name name, unsigned cacheSize, unsigned cacheLatency, unsigned cacheLineSize);

    void cacheAccess() override;
    void printCache() override;
    void calculateBits(unsigned cacheSize, unsigned cacheLineSize) override;
};

#endif
