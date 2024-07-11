#ifndef DIRECTMAPPEDCACHE_H
#define DIRECTMAPPEDCACHE_H

#include "cache.h"

class DirectMappedCache : public Cache
{
public:
    SC_HAS_PROCESS(DirectMappedCache);

    DirectMappedCache(sc_module_name name, unsigned cacheLines, unsigned cacheLineSize, unsigned cacheLatency);

    void cacheAccess() override;
    void printCache() override;
    void calculateBits(unsigned cacheLines, unsigned cacheLineSize) override;
};

#endif // DIRECTMAPPEDCACHE_H
