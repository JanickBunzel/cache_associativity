// cache.cpp
#include "cache.hpp"


void Cache::cache_access() {

    while (true) {

        wait(SC_ZERO_TIME);

        // Adresse aus dem Adressbus lesen
        sc_uint<32> address = addr.read();

        // Offset, Index und Tag aus der Adresse extrahieren
        sc_uint<32> offset = address.range(offset_bits - 1, 0);
        sc_uint<32> index = address.range(offset_bits + index_bits - 1, offset_bits);
        sc_uint<32> tag = address.range(31, offset_bits + index_bits);

        // Cache-Zugriff für direkt abgebildeten Cache simulieren
        if (direct_mapped == 1) {

            // Lese Zugriff
            if (we.read() == 0) {
                if (cache[index] == tag) {
                    std::cout << "Cache hit" << std::endl;
                }
                else {
                    // Wenn der Tag nicht mit dem Index übereinstimmt, handelt es sich um einen Cache-Miss
                    // Also wird der Tag am richtigen Index in den Cache geschrieben
                    // Hier beötigen wir keine LRU-Strategie, da der Cache direkt abgebildet ist
                    cache[index] = tag;
                    std::cout << "Cache miss" << std::endl;
                }
            }

            // Schreib Zugriff
            else {
                // Schreibe den Tag an den Index
                // Hier beötigen wir keine LRU-Strategie, da der Cache direkt abgebildet ist
                cache[index] = tag;
                std::cout << "Cache write" << std::endl;
            }

        }


        // Cache-Zugriff für 4-fach assoziativen Cache simulieren
        else {

            // Lese Zugriff
            if (we.read() == 0) {
                
            }

            // Schreib Zugriff
            else {
                
            }

        }

        wait();
    }

}