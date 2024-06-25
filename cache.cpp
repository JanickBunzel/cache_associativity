#include "cache.hpp"

/**
 * @brief Updates the LRU (Least Recently Used) values for a cache line.
 *
 * This method performs the LRU replacement policy on a set of cache lines. It updates the
 * LRU values for a cache set based on the provided index. If the index is out of bounds,
 * the method will print an error message and terminate the program.
 *
 * @param index_to_update The index of the cache line to be updated.
 *
 * The LRU replacement policy works as follows:
 * - The cache set is determined by dividing the index by 4.
 * - The starting index of the cache set in the LRU array is calculated.
 * - The method iterates through the cache set and decrements the LRU value of each cache line
 *   that has an LRU value greater than the cache line at the given index.
 * - The LRU value of the cache line at the given index is set to 3, indicating it is the most
 *   recently used.
 *
 * @note The method assumes that the LRU array is organized in sets of 4 cache lines.
 * @note If the provided index is out of the valid range, the program will terminate with an error message.
 *
 * @example If the cache set is {3, 2, 1, 0} and the index_to_update is 2, the updated set will be {2, 1, 3, 0}.
 */
void Cache::lru_replacement(unsigned index_to_update)
{
    if (index_to_update >= cache_lines || index_to_update < 0)
    {
        std::cerr << "Error: You are trying to access an Element in lru[] that is out of bounds" << std::endl;
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

/**
 * @brief Simulates the cache access for both direct-mapped and 4-way set associative caches.
 *
 * This method continuously monitors the address bus and simulates cache accesses based on
 * the provided configuration (either direct-mapped or 4-way set associative). It handles both
 * read and write operations, updates the cache contents, and maintains the LRU (Least Recently Used)
 * replacement strategy for the associative cache.
 *
 * For a direct-mapped cache, the method:
 * - Extracts the offset, index, and tag from the address.
 * - Checks if the tag matches the tag stored in the cache at the given index.
 * - Logs a cache hit if the tags match.
 * - Logs a cache miss and updates the cache with the new tag if the tags do not match.
 *
 * For a 4-way set associative cache, the method:
 * - Extracts the offset, index, and tag from the address.
 * - Calculates the set index for the cache.
 * - Searches the set for a matching tag and logs a cache hit if found, updating the LRU strategy.
 * - If the tag is not found (cache miss), it searches for a free block in the set:
 *     - If a free block is found, it writes the new tag to the free block and updates the LRU strategy.
 *     - If no free block is found, it identifies the least recently used block, replaces it with the new tag,
 *       and updates the LRU strategy.
 *
 * The method supports both read and write operations by differentiating based on the write enable signal (we).
 * It waits for the next event or address change in each iteration, ensuring real-time simulation behavior.
 */
void Cache::cache_access()
{

    while (true)
    {
        // Wait for next delty cycle because values form test bench are not propagated yet
        wait(SC_ZERO_TIME);

        // Read address from the address bus
        sc_uint<32> address = addr.read();

        // Extract offset, index, and tag from the address
        sc_uint<32> offset = address.range(offset_bits - 1, 0);
        sc_uint<32> index = address.range(offset_bits + index_bits - 1, offset_bits);
        sc_uint<32> tag = address.range(31, offset_bits + index_bits);

        // Simulate cache access for direct-mapped cache
        if (direct_mapped)
        {
            // Read access
            if (we.read() == 0)
            {
                // Check if the tag in the cache matches the address tag CACHE HIT
                if (cache[index] == tag)
                {
                    // TODO: Wait x cycles
                    std::cout << "CACHE HIT reading adress: " << address << std::endl;
                }
                // Insert value that was not found CACHE MISS
                else
                {
                    // TODO: Wait x + y cycles
                    cache[index] = tag;
                    std::cout << "CACHE MISS reading adress: " << address << std::endl;
                }
            }
            // Write access
            else
            {
                // Write the new tag to the cache at the given index
                cache[index] = tag;
                std::cout << "CACHE WRITE writing adress: " << address << std::endl;
            }
        }
        // Simulate cache access for 4-way set associative cache
        else
        {
            // Calculate the set index for the 4-way set associative cache
            unsigned set_index = index * 4;
            // Read access
            if (we.read() == 0)
            {
                // Search for the tag in the set
                int index_of_tag = search_tag_in_set(tag, set_index);

                // CACHE HIT. Element that is accessed gets updatet to most recently used
                if (index_of_tag != -1)
                {
                    lru_replacement(index_of_tag);
                    std::cout << "CACHE HIT reading adress: " << address << std::endl;
                }
                // CACHE MISS
                else
                {
                    // Search for a free line in the set
                    int index_of_free_line = search_free_line_in_set(set_index);

                    // Free line found. Write the new tag to the free line. Update LRU strategy
                    if (index_of_free_line != -1)
                    {
                        cache[index_of_free_line] = tag;
                        lru_replacement(index_of_free_line);
                        std::cout << "CACHE MISS with free cacheline reading address: " << address << std::endl;
                    }
                    // No free line available. Write the new tag to the least recently used line. Update LRU strategy
                    else
                    {
                        // Find the least recently used block in the set
                        int index_of_lru_line = search_least_recently_used_line(set_index);
                        cache[index_of_lru_line] = tag;
                        lru_replacement(index_of_lru_line);
                        std::cout << "CACHE MISS with full set reading adress: " << address << std::endl;
                    }
                }
            }
            // Write access
            else
            {
                // Search for the tag in the set
                int index_of_tag = search_tag_in_set(tag, set_index);
                // CACHE HIT. Element that is accessed gets updatet to most recently used
                if (index_of_tag != -1)
                {
                    lru_replacement(index_of_tag);
                    std::cout << "CACHE HIT writing value with adress: " << address << std::endl;
                }
                // CACHE MISS
                else
                {

                    // Search for a free line in the set
                    int index_of_free_line = search_free_line_in_set(set_index);

                    // Free line found. Write the new tag to the free line. Update LRU strategy
                    if (index_of_free_line != -1)
                    {
                        cache[index_of_free_line] = tag;
                        lru_replacement(index_of_free_line);
                        std::cout << "CACHE MISS with free cacheline writing value with adress: " << address << std::endl;
                    }
                    // No free line available. Write the new tag to the least recently used line. Update LRU strategy
                    else
                    {
                        // Find the least recently used block in the set
                        int index_of_lru_line = search_least_recently_used_line(set_index);
                        cache[index_of_lru_line] = tag;
                        lru_replacement(index_of_lru_line);
                        std::cout << "CACHE MISS writing value with adress: " << address << std::endl;
                    }
                }
            }
        }
        wait();
    }
}

/**
 * @brief Searches for the specified tag within a cache set.
 * 
 * This method searches for a given tag in a 4-way set associative cache set.
 * It iterates through the set to find the index where the tag is located.
 * 
 * @param tag The tag to search for within the cache set.
 * @param set_index The starting index of the cache set to search within.
 * @return The index of the tag within the cache if found, or -1 if the tag is not found in the set.
 */
int Cache::search_tag_in_set(sc_uint<32> tag, unsigned set_index)
{
    int index_of_tag = -1;
    for (int i = 0; i < 4; i++)
    {
        if (cache[set_index + i] == tag)
        {
            index_of_tag = set_index + i;
            break;
        }
    }
    return index_of_tag;
}


/**
 * @brief Searches for a free line within a cache set.
 * 
 * This method searches for a free line (an unused block) within a 4-way set associative cache set.
 * It iterates through the set to find an index where the LRU indicator is -1, indicating the line is free.
 * 
 * @param set_index The starting index of the cache set to search within.
 * @return The index of the free line within the cache if found, or -1 if no free line is available in the set.
 */
int Cache::search_free_line_in_set(unsigned set_index)
{
    int index_of_free_line = -1;
    for (int i = 0; i < 4; i++)
    {
        if (lru[set_index + i] == -1)
        {
            index_of_free_line = set_index + i;
            break;
        }
    }
    return index_of_free_line;
}

/**
 * @brief Searches for the least recently used (LRU) line within a cache set.
 * 
 * This method searches for the least recently used line within a 4-way set associative cache set.
 * It iterates through the set to find an index where the LRU indicator is 0, indicating the least recently used line.
 * 
 * @param set_index The starting index of the cache set to search within.
 * @return The index of the least recently used line within the cache if found, or -1 if no LRU line is identified.
 */
int Cache::search_least_recently_used_line(unsigned set_index)
{
    int index_of_lru_line = -1;
    for (int i = 0; i < 4; i++)
    {
        if (lru[set_index + i] == 0)
        {
            index_of_lru_line = set_index + i;
            break;
        }
    }
    return index_of_lru_line;
}