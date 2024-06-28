#ifndef CACHE_HPP
#define CACHE_HPP
#include <systemc.h>

SC_MODULE(Cache)
{
private:
    // Cache Configuration
    unsigned int cache_lines;     // Number of cache lines
    unsigned int cache_line_size; // In bytes
    int direct_mapped;            // 1 -> direct mapped, 0 -> fourway
    unsigned int cacheLatency;   // In cycles
    unsigned int memoryLatency;  // In cycles

    // Array of usage ranks for LRU replacement strategy
    sc_int<32> *lru;

    // DirectMapped cache, array of cache lines
    sc_uint<32> *cache;

    // Fourway cache configuration
    int num_sets = cache_lines / 4;

    // Numbers of bits represented in an address:
    unsigned offset_bits; // - offset_bits: Bits for the position of a block in a cache line
    unsigned index_bits;  // - index_bits: Bits for the position of a set in the cache
    unsigned tag_bits;    // - tag_bits: Bits for the tag of a cache line

public:
    // SystemC Input Ports:
    // - Clock Signal
    sc_in<bool> clk;

    // - Address of the next request
    sc_in<sc_uint<32>> addr;
    // - Value of the next request
    sc_in<sc_uint<32>> wdata;
    // - Write or read request
    sc_in<bool> we;

    // Method declarations
    void cache_access();
    void lru_replacement(unsigned index);
    int search_tag_in_set(sc_uint<32> tag, unsigned set_index);
    int search_free_line_in_set(unsigned set_index);
    int search_least_recently_used_line(unsigned set_index);

    // Standardkonstruktor für die Cache-Klasse.
    SC_CTOR(Cache);

    /**
     * Konstruktor für die Cache-Klasse.
     *
     * Initialisiert eine Instanz der Cache-Klasse mit spezifischen Parametern für die Cache-Konfiguration.
     *
     * @param name Der Name des Moduls, wird an den sc_module Konstruktor weitergegeben.
     * @param directMapped Gibt an, ob der Cache direkt abgebildet ist (!= 0) oder 4-fach assoziativ (0).
     * @param cacheLines Die Anzahl der Cache-Lines.
     * @param cacheLineSize Die Größe einer Cache-Line in Byte.
     */
    Cache(sc_module_name name, int directMapped, unsigned chacheLines, unsigned chacheLineSize, unsigned cacheLatency, unsigned memoryLatency)
        : sc_module(name), direct_mapped(directMapped), cache_lines(chacheLines), cache_line_size(chacheLineSize), cacheLatency(cacheLatency), memoryLatency(memoryLatency)
    {

        // Allocate memory for the cache array
        cache = new sc_uint<32>[cache_lines];

        // Allocate lru array and initialize with -1 representing unused cache lines
        lru = new sc_int<32>[cache_lines];
        for (int i = 0; i < cache_lines; i++)
        {
            lru[i] = -1;
        }

        // Set bit format properties
        offset_bits = log2(cache_line_size);
        index_bits = log2(direct_mapped ? cache_lines : num_sets);
        tag_bits = 32 - offset_bits - index_bits;

        // Set the thread for a request that accesses the cache
        SC_THREAD(cache_access);
        dont_initialize();
        sensitive << clk.pos();
    }

    // Destructor
    ~Cache()
    {
        // Free allocated memory
        delete[] cache;
    }
};

#endif