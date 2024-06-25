// cache.cpp
#include "cache.hpp"

void Cache::lru_replacement(unsigned index_to_update)
{

    unsigned cache_set = index_to_update / 4;
    unsigned cache_set_index = 4 * cache_set;

    for (int i = 0; i < 4; i++)
    {

        if (lru[cache_set_index + i] >= lru[index_to_update])
        {
            lru[cache_set_index + i] = lru[cache_set_index + i] - 1;
        }
    }

    lru[index_to_update] = 3;
}

void Cache::cache_access()
{

    while (true)
    {

        wait(SC_ZERO_TIME);

        // Adresse aus dem Adressbus lesen
        sc_uint<32> address = addr.read();

        // Offset, Index und Tag aus der Adresse extrahieren
        sc_uint<32> offset = address.range(offset_bits - 1, 0);
        sc_uint<32> index = address.range(offset_bits + index_bits - 1, offset_bits);
        sc_uint<32> tag = address.range(31, offset_bits + index_bits);

        // Cache-Zugriff für direkt abgebildeten Cache simulieren
        if (direct_mapped)
        {

            // Lese Zugriff
            if (we.read() == 0)
            {
                if (cache[index] == tag)
                {
                    std::cout << "Cache hit" << std::endl;
                }
                else
                {
                    // Wenn der Tag nicht mit dem Index übereinstimmt, handelt es sich um einen Cache-Miss
                    // Also wird der Tag am richtigen Index in den Cache geschrieben
                    // Hier beötigen wir keine LRU-Strategie, da der Cache direkt abgebildet ist
                    cache[index] = tag;
                    std::cout << "Cache miss" << std::endl;
                }
            }

            // Schreib Zugriff
            else
            {
                // Schreibe den Tag an den Index
                // Hier beötigen wir keine LRU-Strategie, da der Cache direkt abgebildet ist
                cache[index] = tag;
                std::cout << "Cache write" << std::endl;
            }
        }

        // Cache-Zugriff für 4-fach assoziativen Cache simulieren
        else
        {

            // Lese Zugriff
            if (we.read() == 0)
            {

                // Index für das Set berechnen
                unsigned set_index = index * 4;

                // Suche nach dem Tag im Set
                int index_to_update = -1;
                for (int i = 0; i < 4; i++)
                {
                    if (cache[set_index + i] == tag)
                    {
                        index_to_update = set_index + i;
                        break;
                    }
                }

                // Cache Hit
                if (index_to_update != -1)
                {
                    // Es wird lediglich die LRU-Strategie aktualisiert
                    lru_replacement(index_to_update);
                    std::cout << "Cache hit" << std::endl;
                }

                // Cache Miss
                else
                {

                    // Suche nach einem freien Block im Set
                    int free_line_index = -1;
                    for (int i = 0; i < 4; i++)
                    {
                        if (lru[set_index + i] == -1)
                        {
                            free_line_index = set_index + i;
                            break;
                        }
                    }

                    // Es gibt einen freien Block
                    if (free_line_index != -1)
                    {
                        // Schreibe den Tag in den freien Block
                        cache[free_line_index] = tag;
                        // Aktualisiere die LRU-Strategie
                        lru_replacement(free_line_index);

                        std::cout << "Cache miss mit freier Cachezeile" << std::endl;
                    }

                    // Es gibt keinen freien Block
                    else
                    {
                        // Suche nach dem Block, der am längsten nicht benutzt wurde
                        int lru_line_index = -1;
                        for (int i = 0; i < 4; i++)
                        {
                            if (lru[set_index + i] == 0)
                            {
                                lru_line_index = set_index + i;
                                break;
                            }
                        }

                        // Schreibe den Tag in den LRU-Block
                        cache[lru_line_index] = tag;
                        // Aktualisiere die LRU-Strategie
                        lru_replacement(lru_line_index);
                        std::cout << "Cache miss ohne freie Cachzeile" << std::endl;
                    }
                }
            }

            // Schreib Zugriff
            else
            {

                // Index für das Set berechnen
                unsigned set_index = index * 4;

                // Suche nach dem Tag im Set
                int index_to_update = -1;
                for (int i = 0; i < 4; i++)
                {
                    if (cache[set_index + i] == tag)
                    {
                        index_to_update = set_index + i;
                        break;
                    }
                }

                // Cache Hit
                if (index_to_update != -1)
                {
                    // Es wird lediglich die LRU-Strategie aktualisiert
                    lru_replacement(index_to_update);
                    std::cout << "Cache hit beim schreiben" << std::endl;
                }

                // Cache Miss
                else
                {

                    // Suche nach einem freien Block im Set
                    int free_line_index = -1;
                    for (int i = 0; i < 4; i++)
                    {
                        if (lru[set_index + i] == -1)
                        {
                            free_line_index = set_index + i;
                            break;
                        }
                    }

                    // Es gibt einen freien Block
                    if (free_line_index != -1)
                    {
                        // Schreibe den Tag in den freien Block
                        cache[free_line_index] = tag;
                        // Aktualisiere die LRU-Strategie
                        lru_replacement(free_line_index);

                        std::cout << "Cache miss mit freier Cachezeile beim Schreiben" << std::endl;
                    }

                    // Es gibt keinen freien Block
                    else
                    {
                        // Suche nach dem Block, der am längsten nicht benutzt wurde
                        int lru_line_index = -1;
                        for (int i = 0; i < 4; i++)
                        {
                            if (lru[set_index + i] == 0)
                            {
                                lru_line_index = set_index + i;
                                break;
                            }
                        }

                        // Schreibe den Tag in den LRU-Block
                        cache[lru_line_index] = tag;
                        // Aktualisiere die LRU-Strategie
                        lru_replacement(lru_line_index);
                        std::cout << "Cache miss ohne freie Cachzeile beim Schreiben" << std::endl;
                    }
                }
            }

            wait();
        }
    }
}