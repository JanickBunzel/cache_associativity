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

    CacheLine& getCacheline(unsigned setIndex, unsigned cachelineIndex);
    int searchTagInSet(sc_uint<32> tag, unsigned setIndex);
    int searchFreeLineInSet(unsigned setIndex);
    int searchLeastRecentlyUsedLineInSet(unsigned setIndex);
    void updateLruIndicesInSet(unsigned setIndex, unsigned cachelineIndexToUpdate);
};

#endif // FOURWAYMAPPEDCACHE_H
