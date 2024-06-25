#ifndef CACHE_HPP
#define CACHE_HPP
#include <systemc.h>

SC_MODULE(Cache) {

    // ################ Ports ################ //

    // Adresse, die von der CPU angefordert wird
    sc_in<sc_uint<32>> addr;
    // Daten, die von der CPU geschrieben werden
    sc_in<sc_uint<32>> wdata;
    // Clocksignal
    sc_in<bool> clk;
    // Write Enable Signal
    sc_in<bool> we;

    void cache_access();

    void lru_replacement(unsigned index);

    int search_tag_in_set(sc_uint<32> tag, unsigned set_index);

    int search_free_line_in_set(unsigned set_index);

    int search_least_recently_used_line(unsigned set_index);

    /**
     * Standardkonstruktor für die Cache-Klasse.
     */
    SC_CTOR(Cache);

    /**
    * Konstruktor für die Cache-Klasse.
    *
    * Initialisiert eine Instanz der Cache-Klasse mit spezifischen Parametern für die Cache-Konfiguration.
    *
    * @param name Der Name des Moduls, wird an den sc_module Konstruktor weitergegeben.
    * @param directMapped Gibt an, ob der Cache direkt abgebildet ist (1) oder nicht (0).
    * @param cacheLines Die Anzahl der Cache-Lines.
    * @param cacheLineSize Die Größe einer Cache-Line in Byte.
    */
    Cache (sc_module_name name, int directMapped, unsigned chacheLines, unsigned chacheLineSize) : sc_module(name) , direct_mapped(directMapped), cache_lines(chacheLines), cache_line_size(chacheLineSize){
        
        // Speicher für Cache allozieren
        cache = new sc_uint<32>[cache_lines];

        // LRU Speicher für 4-fach assoziativen Cache allozieren und mit -1 initialisieren
        lru = new sc_int<32>[cache_lines];
        for (int i = 0; i < cache_lines; i++) {
            lru[i] = -1;
        }

        // Offset-Bits berechnen
        offset_bits = log2(cache_line_size);
        // Index-Bits berechnen
        if (direct_mapped == 1) {
            // Direct-Mapped Cache berechnet Index-Bits aus der Anzahl der Cache-Lines
            index_bits = log2(cache_lines);
        }
        else {
            // 4-Fach assoziativer Cache berechnet Index-Bits aus der Anzahl der Sets
            index_bits = log2(num_sets);
        }
        // Tag-Bits berechnen
        tag_bits = 32 - offset_bits - index_bits;

        SC_THREAD(cache_access);
        dont_initialize();  
        sensitive << clk.pos();
    }

    // Destruktor
    ~Cache(){

        // Speicher freigeben (Array)
        delete[] cache;
    }

private:
    // ################ Allgemeine Cache Eigenschaften ################ //

    // Größe des Caches in Lines
    unsigned cache_lines;
    // Größer einer Line in Byte
    unsigned cache_line_size;
    // Flag, ob der Cache direct-mapped ist
    int direct_mapped;

    // ################ LRU Ersetzungsstrategie ################ //

    sc_int<32>* lru;

    // ################  4-Fach assoziativer Cache ################ //

    // Anzahl der Blöcke pro Set
    unsigned lines_per_set = 4;
    // Anzahl der Sets
    int num_sets = cache_lines / lines_per_set;

    // ################ Cache-Speicher ################ //

    // Direct-Mapped Cache Speicher, in dem nur die Tag-Informationen gespeichert werden
    sc_uint<32>* cache;

    // ################ Cache-Zugriffs Attribute ################ //

    // Offset-Bits (Anzahl der Bits, die für den Offset benötigt werden)
    unsigned offset_bits;
    // Index-Bits (Anzahl der Bits, die für den Index benötigt werden)
    unsigned index_bits;
    // Tag-Bits (Anzahl der Bits, die für den Tag benötigt werden)
    unsigned tag_bits;

};
#endif