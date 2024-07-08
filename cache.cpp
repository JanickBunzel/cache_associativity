#include "cache.hpp"

void Cache::cache_access()
{
    while (!cpuDone.read())
    {
        // Wait for next delty cycle because values from the cpu are not propagated yet
        wait(SC_ZERO_TIME);

        // set done signal to false inndicating that the cache is processing the request
        cacheDone.write(false);

        // Extract offset, index, and tag from the address bus
        sc_uint<32> address = addr.read();
        sc_uint<32> offset = address.range(offset_bits - 1, 0);
        sc_uint<32> index = address.range(offset_bits + index_bits - 1, offset_bits);
        sc_uint<32> tag = address.range(31, offset_bits + index_bits);
        bool isWrite = we.read() == 1;

        // Determine if the request is a cache hit or miss
        bool cacheHit = direct_mapped ? directmapped_access(index, tag) : fourway_access(index, tag);

        // Update statistics
        if (cacheHit)
        {
            hits++;
        }
        else
        {
            misses++;
        }

        // Print feedback to the console
        std::cout << "[Cache]: " << (cacheHit ? "HIT " : "MISS ") << (isWrite ? "writing" : "reading") << " address: " << address << std::endl;

        // Simulate the cache and memory access latency
        int waitCycles = cacheHit ? cacheLatency : (cacheLatency + memoryLatency);
        for (int i = 0; i < waitCycles; i++)
        {
            wait();
        }

        // Set done signal to true indicating that the cache has finished processing the request
        cacheDone.write(true);

        // Wait for the clock tick
        wait();
    }
}

bool Cache::directmapped_access(int index, int tag)
{
    // Determine if the request is a cache hit or miss
    bool cacheHit = cache[index] == tag;
    
    if (!cacheHit)
    {
        // If a miss, insert value that was not found directly
        cache[index] = tag;
    }

    return cacheHit;
}

bool Cache::fourway_access(int index, int tag)
{
    int set_index = index * 4;
    int index_of_tag = -1;
    
    // Search for the tag in the set
    for (int i = 0; i < 4; i++)
    {
        if (cache[set_index + i] == tag)
        {
            index_of_tag = set_index + i;
            break;
        }
    }
    
    // Determine if the request is a cache hit or miss
    bool cacheHit = (index_of_tag != -1);

    if (cacheHit)
    {
        lru_replacement(index_of_tag);
    }
    else
    {
        int target_index = -1;

        // Search for a free line in the set
        for (int i = 0; i < 4; i++)
        {
            if (lru[set_index + i] == -1)
            {
                target_index = set_index + i;
                break;
            }
        }

        // If no free line found, search for the least recently used line
        if (target_index == -1)
        {
            for (int i = 0; i < 4; i++)
            {
                if (lru[set_index + i] == 0)
                {
                    target_index = set_index + i;
                    break;
                }
            }
        }

        cache[target_index] = tag;
        lru_replacement(target_index);
    }

    return cacheHit;
}

void Cache::lru_replacement(unsigned index_to_update)
{
    if (index_to_update >= cache_lines || index_to_update < 0)
    {
        std::cerr << "[Cache]: Error: You are trying to access an Element in lru[] that is out of bounds" << std::endl;
        std::exit(EXIT_FAILURE);
    }

    unsigned cache_set = index_to_update / 4;
    unsigned cache_set_index = 4 * cache_set;

    for (int i = 0; i < 4; i++)
    {
        if (lru[cache_set_index + i] > lru[index_to_update])
        {
            lru[cache_set_index + i] = lru[cache_set_index + i] - 1;
        }
    }

    lru[index_to_update] = 3;
}
