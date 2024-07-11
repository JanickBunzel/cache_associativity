#ifndef FOURWAYMAPPEDCACHE_H
#define FOURWAYMAPPEDCACHE_H

#include "cache.h"

class FourwayMappedCache : public Cache
{
public:
    SC_HAS_PROCESS(FourwayMappedCache);

    FourwayMappedCache(sc_module_name name, unsigned cacheLines, unsigned cacheLineSize, unsigned cacheLatency);

    void cacheAccess() override;
    void printCache() override;
    void calculateBits(unsigned cacheLines, unsigned cacheLineSize) override;
};

#endif // FOURWAYMAPPEDCACHE_H
