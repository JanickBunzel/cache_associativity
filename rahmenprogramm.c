#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <string.h>
#include <stdarg.h>
#include "Request.h"
#include "Result.h"

// Struct to store properties of the cache
typedef struct {
    int cycles;
    int directmapped;
    int fourway;
    int cacheline_size;
    int cachelines;
    int cache_latency;
    int memory_latency;
    char *tracefile;
    char *eingabedatei;
} CacheConfig;

// Default cache configuration
CacheConfig cache_config = {
    .cycles = 2800, // allows the cache to process 1000 requests (with a hit rate of 90%, the average cycle count per hit is 2.8)
    .directmapped = 1, // Using direct mapped cache as default
    .fourway = 0, // Using direct mapped cache as default
    .cacheline_size = 64, // Standard size that balances spatial locality and overhead
    .cachelines = 128, // with each line of 64 bytes, this provides an 8KB cache
    .cache_latency = 2, // quick access for first-level cache
    .memory_latency = 10, // realistic gap between cache and main access time
    .tracefile = "tracefile.vcd", // default tracefile name
    .eingabedatei = NULL, // input file needs to be provided
};

int MAX_REQUEST_LINE_LENGTH = 256;

// Function declarations
CacheConfig arguments_to_cache_config(int argc, char *argv[]);
void print_cache_config(CacheConfig cacheConfig);
int count_lines_in_file(char* filename);
struct Request* read_requests_from_file(int number_of_requests, char* filename);
void print_help_and_exit_with_error(char* errorMessage, ...);
void print_help();
int is_integer(char* str);

// Declaration of the extern C++ function
#ifdef __cplusplus
extern "C" {
#endif

struct Result run_simulation(
    int cycles,
    int directMapped,
    unsigned cacheLines,
    unsigned cacheLineSize,
    unsigned cacheLatency,
    unsigned memoryLatency,
    size_t numRequests,
    struct Request requests[],
    const char* tracefile);

#ifdef __cplusplus
}
#endif

/* 
    TODO: Remove this comment
    Beispiele:
    ./linked_program -c 1000 --directmapped --cacheline-size 64 --cachelines 16 --cache-latency 2 --memory-latency 10 --tf tracefile.vcd r.csv
    ./rahmenprogramm -c 1000 --directmapped --cacheline-size 64 --cachelines 16 --cache-latency 2 --memory-latency 10 --tf tracefile.vcd r.csv
*/ 
int main(int argc, char *argv[]) {
    // Process the given parameters
    CacheConfig cacheConfig = arguments_to_cache_config(argc, argv);
    
    // Feedback
    print_cache_config(cacheConfig);
    
    // Count the requests
    int number_of_requests = count_lines_in_file(cacheConfig.eingabedatei);

    // Process the read and write requests
    struct Request* requests = read_requests_from_file(number_of_requests, cacheConfig.eingabedatei);

    // Run the SystemC simulation
    Result result = run_simulation(
        cacheConfig.cycles,
        cacheConfig.directmapped,
        cacheConfig.cachelines,
        cacheConfig.cacheline_size,
        cacheConfig.cache_latency,
        cacheConfig.memory_latency,
        number_of_requests,
        requests,
        cacheConfig.tracefile
    );

    // Print the result
    printf("Simulation Result:\n");
    printf("  - Cache hits: %zu\n", result.cycles);
    printf("  - Cache misses: %zu\n", result.misses);
    printf("  - Cache writes: %zu\n", result.hits);
    printf("  - Memory accesses: %zu\n", result.primitiveGateCount);

    // Free allocated space
    free(requests);

    return 0;
}

// Converts the given arguments to a CacheConfig struct
CacheConfig arguments_to_cache_config(int argc, char *argv[]) {
    // Set cache types to 0 to read users cache type input independently from the default cache config
    int default_directmapped = cache_config.directmapped;
    int default_fourway = cache_config.fourway;
    cache_config.directmapped = 0;
    cache_config.fourway = 0;
    
    // Loop depending on number of arguments (controlled using getopt_long)
    while (1) {
        const char* optstring = "c:dfs:l:a:m:t:h";
        static struct option longopts[] = {
            {"cycles", required_argument, NULL, 'c'},
            {"directmapped", no_argument, NULL, 'd'},
            {"fourway", no_argument, NULL, 'f'},
            {"cacheline-size", required_argument, NULL, 's'},
            {"cachelines", required_argument, NULL, 'l'},
            {"cache-latency", required_argument, NULL, 'a'},
            {"memory-latency", required_argument, NULL, 'm'},
            {"tf", required_argument, NULL, 't'},
            {"help", no_argument, NULL, 'h'},
            {0, 0, 0, 0} // End of arguments
        };

        int next_opt = getopt_long(argc, argv, optstring, longopts, NULL);

        if (next_opt == -1) {
            // No more arguments
            break;
        }

        // Processing each argument with getopt_long, while checking for invalid cases
        switch (next_opt) {
            case 'c':
                // Argument: Cycles, Expected: int > 0
                if (!is_integer(optarg) || atoi(optarg) <= 0) {
                    print_help_and_exit_with_error("Error: Die Anzahl der Zyklen muss ein Integer sein und größer als 0\n");
                }
                cache_config.cycles = atoi(optarg);
                break;
            case 'd':
                // Argument: foruway, Expected: bool (Can't be used with directmapped)
                if (cache_config.fourway) {
                    print_help_and_exit_with_error("Error: Es kann nur ein Cache Typ ausgewählt werden (directmapped oder fourway)\n");
                }
                cache_config.directmapped = 1;
                break;
            case 'f':
                // Argument: fourway, Expected: bool (Can't be used with directmapped)
                if (cache_config.directmapped) {
                    print_help_and_exit_with_error("Error: Es kann nur ein Cache Typ ausgewählt werden (directmapped oder fourway)\n");
                }
                cache_config.fourway = 1;
                break;
            case 's':
                // Argument: Cacheline size, Expected: int > 0
                if (!is_integer(optarg) || atoi(optarg) <= 0) {
                    print_help_and_exit_with_error("Error: Die Größe einer Cachezeile muss ein Integer sein und größer als 0\n");
                }
                cache_config.cacheline_size = atoi(optarg);
                break;
            case 'l':
                // Argument: Cachelines, Expected: int > 0
                if (!is_integer(optarg) || atoi(optarg) <= 0) {
                    print_help_and_exit_with_error("Error: Die Anzahl der Cachelines muss ein Integer sein und größer als 0\n");
                }
                cache_config.cachelines = atoi(optarg);
                break;
            case 'a':
                // Argument: Cache Latency, Expected: int > 0
                if (!is_integer(optarg) || atoi(optarg) <= 0) {
                    print_help_and_exit_with_error("Error: Die Cache Latency muss ein Integer sein und größer als 0\n");
                }
                cache_config.cache_latency = atoi(optarg);
                break;
            case 'm':
                // Argument: Memory Latency, Expected: int > 0 && > cache_latency
                if (!is_integer(optarg) || atoi(optarg) <= 0) {
                    print_help_and_exit_with_error("Error: Die Memory Latency muss ein Integer sein und größer als 0 und die Cache Latency sein\n");
                }
                cache_config.memory_latency = atoi(optarg);
                break;
            case 't':
                // Argument: Tracefile, Expected: char* with length < 255
                if (strlen(optarg) > 255) {
                    print_help_and_exit_with_error("Error: Der Dateiname für das Tracefile darf maximal 255 Zeichen lang sein\n");
                }
                cache_config.tracefile = optarg;
                break;
            
            case 'h':
                // If the help argument is passed, ignore all other arguments and print help
                print_help();
                exit(0);

            case '?':
            default:
                print_help_and_exit_with_error("Error: Unbekanntes Argument\n");
        }
    }

    // Set default cache type if no type is given
    if (cache_config.directmapped == 0 && cache_config.fourway == 0) {
        cache_config.directmapped = default_directmapped;
        cache_config.fourway = default_fourway;
    }

    // The last argument is the Eingabedatei (independent from its argument position)
    if (optind >= argc) {
        // If there is no more argument, the Eingabedatei is missing
        print_help_and_exit_with_error("Error: Keine Eingabedatei angegeben\n");
    } else {
        if (access(argv[optind], F_OK) != 0) {
            // Check that the file with the given path exists
            print_help_and_exit_with_error("Error: Die Eingabedatei wurde nicht gefunden.\n");
        }
        cache_config.eingabedatei = argv[optind];
    }

    // All Invalid cases checked
    return cache_config;
}

// Print the given cache configuration
void print_cache_config(CacheConfig cache_config) {
    printf("Cache Configuration:\n");
    printf("  - Cycles: %d\n", cache_config.cycles);
    printf("  - Direct mapped: %d\n", cache_config.directmapped);
    printf("  - Four way: %d\n", cache_config.fourway);
    printf("  - Cacheline size: %d\n", cache_config.cacheline_size);
    printf("  - Cachelines: %d\n", cache_config.cachelines);
    printf("  - Cache latency: %d\n", cache_config.cache_latency);
    printf("  - Memory latency: %d\n", cache_config.memory_latency);
    printf("  - Tracefile: %s\n", cache_config.tracefile ? cache_config.tracefile : "None");
    printf("  - Eingabedatei: %s\n\n", cache_config.eingabedatei);
}

// Counts the lines in the given file
int count_lines_in_file(char* filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        print_help_and_exit_with_error("Error: Fehler beim Öffnen der Datei: %s\n",  filename);
    }

    int lines = 0;
    char line[256];
    while (fgets(line, sizeof(line), file)) {
        lines++;
    }

    fclose(file);

    return lines;
}

// Reads the requests from the given .csv file
struct Request* read_requests_from_file(int number_of_requests, char* filename) {
    // Allocate memory for the requests array
    struct Request* requests = malloc(number_of_requests * sizeof(struct Request));

    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        print_help_and_exit_with_error("Error: Fehler beim Öffnen der Datei: %s\n",  filename);
    }

    int index = 0;
    char line[MAX_REQUEST_LINE_LENGTH];
    while (fgets(line, sizeof(line), file)) {
        char rw, addr[MAX_REQUEST_LINE_LENGTH], data[MAX_REQUEST_LINE_LENGTH], rest[MAX_REQUEST_LINE_LENGTH];
        
        // Scan the format of the current line and store the entries in the local variables
        int number_of_entries = sscanf(line, "%c,%[^,],%s", &rw, addr, data);

        // Check if the line is in the correct format
        if (number_of_entries == 2 || number_of_entries == 3) {
            if (number_of_entries == 3 && rw == 'R') {
                // write request expected for 3 entries
                print_help_and_exit_with_error("Error in Zeile %d: Bei einem Lesezugriff darf kein Wert übergeben werden\n", index + 1);
            }
            else if (number_of_entries == 2 && rw == 'W') {
                // read request expected for 2 entries
                print_help_and_exit_with_error("Error in Zeile %d: Bei einem Schreibzugriff muss ein Wert übergeben werden\n", index + 1);
            }
        } else {
            // invalid line format for more than 3 or less than 2 entries
            print_help_and_exit_with_error("Error: Zeile %d ist nicht im Format <W/R, Adresse, Wert>: %s\n", index + 1, line);
        }

        // Store the entries in the current request, converting addr and data from hex or decimal string to uint32_t
        requests[index].addr = (uint32_t)strtol(addr, NULL, strncmp(addr, "0x", 2) == 0 ? 16 : 10);
        requests[index].data = (uint32_t)strtol(data, NULL, strncmp(data, "0x", 2) == 0 ? 16 : 10);
        requests[index].we = rw == 'W' ? 1 : 0;

        index++;
    }

    fclose(file);

    return requests;
}

// Variadic function to print the error and help message and exits the program
void print_help_and_exit_with_error(char *errorMessage, ...) {
    va_list args;
    va_start(args, errorMessage);

    fprintf(stderr, "Error: ");
    vfprintf(stderr, errorMessage, args);

    va_end(args);

    print_help();
    exit(1);
}

// Prints the description and usage of the arguments
void print_help() {
    printf("\nUsage: rahmenprogramm [OPTIONS] <Eingabedatei>\n");
    printf("Options:\n");
    printf("  -c, --cycles <Zahl>         Die Anzahl der Zyklen, die simuliert werden sollen.\n");
    printf("  --directmapped              Simuliert einen direkt assoziativen Cache.\n");
    printf("  --fourway                   Simuliert einen vierfach assoziativen Cache.\n");
    printf("  --cacheline-size <Zahl>     Die Größe einer Cachezeile in Byte.\n");
    printf("  --cachelines <Zahl>         Die Anzahl der Cachezeilen.\n");
    printf("  --cache-latency <Zahl>      Die Latenzzeit eines Caches in Zyklen.\n");
    printf("  --memory-latency <Zahl>     Die Latenzzeit des Hauptspeichers in Zyklen.\n");
    printf("  --tf <Dateiname>            Ausgabedatei für ein Tracefile mit allen Signalen.\n");
    printf("  -h, --help                  Eine Beschreibung aller Optionen des Rahmenprogramms und Verwendung ausgeben und das Programm danach beendet.\n");
    printf("  Positional Arguments:\n");
    printf("  <Eingabedatei>              Die .csv Eingabedatei, die die zu verarbeitenden Daten enthält. Zeilenformat: <W, Adresse, Wert> oder <R, Adresse>\n");
}

// Checks if a string is an integer
int is_integer(char* str) {
    while (*str != '\0') {
        if (!isdigit(*str)) {
            return 0;
        }
        str++;
    }
    return 1;
}
