#ifndef FOURWAYMAPPEDCACHE_H
#define FOURWAYMAPPEDCACHE_H

#include "cache.h"

class FourwayMappedCache : public Cache
{
public:
    SC_HAS_PROCESS(FourwayMappedCache);

    FourwayMappedCache(sc_module_name name, unsigned cachelines, unsigned cachelineSize, unsigned cacheLatency);

    void cacheAccess() override;
    void printCache() override;
    void calculateBits(unsigned cachelines, unsigned cachelineSize) override;

    int searchTagInSet(sc_uint<32> tag, unsigned setIndex);
    void lruReplacement(unsigned indexToUpdate);
    int searchFreeLineInSet(unsigned setIndex);
    int searchLeastRecentlyUsedLine(unsigned setIndex);
};

#endif // FOURWAYMAPPEDCACHE_H
