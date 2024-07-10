#ifndef DIRECTMAPPEDCACHEPOLY_H
#define DIRECTMAPPEDCACHEPOLY_H

#include "cache.h"

class DirectMappedCachePoly : public Cache
{
public:
    SC_HAS_PROCESS(DirectMappedCachePoly);

    DirectMappedCachePoly(sc_module_name name, unsigned cacheSize, unsigned cacheLatency, unsigned cacheLineSize);

    void cacheAccess() override;
    void printCache() override;
    void calculateBits(unsigned cacheSize, unsigned cacheLineSize) override;
};

#endif
