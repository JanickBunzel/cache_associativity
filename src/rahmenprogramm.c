#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <string.h>
#include <stdarg.h>
#include "request.h"
#include "result.h"

// Struct to store properties of the cache
typedef struct
{
    int cycles;        // Number of cycles to simulate
    int directmapped;  // bool
    int fourway;       // bool
    int cachelines;    // Number of cachelines
    int cachelineSize; // In bytes
    int cacheLatency;  // In cycles
    int memoryLatency; // In cycles
    char *tracefile;
    char *eingabedatei;
} CacheConfig;

// Default cache configuration
CacheConfig cacheConfig = {
    .cycles = 3000,       // allows the cache to process TODO requests (with a hit rate of TODO%, and average cycles/request TODO as below)
    .directmapped = 1,    // Using direct mapped cache as default
    .fourway = 0,         // Using direct mapped cache as default
    .cachelines = 8,     // with each line of TODO bytes, this provides an TODOKB cache
    .cachelineSize = 4,   // Standard size that balances spatial locality and overhead
    .cacheLatency = 5,    // quick access for first-level cache
    .memoryLatency = 10,  // realistic gap between cache and main access time
    .tracefile = "",      // name of the tracefile (optional)
    .eingabedatei = NULL, // input file needs to be provided
};

// Macro to handle request lines in the input file
int MAX_REQUEST_LINE_LENGTH = 256;

// Function declarations
CacheConfig arguments_to_cacheConfig(int argc, char *argv[]);
void print_cacheConfig(CacheConfig cacheConfig);
int count_lines_in_file(char *filename);
Request *read_requests_from_file(int numRequests, char *filename);
void print_help_and_exit_with_error(char *errorMessage, ...);
void print_help();
int is_integer(char *str);

// Declaration of the extern C++ function
#ifdef __cplusplus
extern "C"
{
#endif

    Result run_simulation(
        int cycles,
        int directMapped,
        unsigned cacheLines,
        unsigned cacheLineSize,
        unsigned cacheLatency,
        unsigned memoryLatency,
        size_t numRequests,
        Request requests[],
        const char *tracefile);

#ifdef __cplusplus
}
#endif

int main(int argc, char *argv[])
{
    // Process the given parameters
    CacheConfig cacheConfig = arguments_to_cacheConfig(argc, argv);

    // Feedback
    print_cacheConfig(cacheConfig);

    // Count the requests
    int numRequests = count_lines_in_file(cacheConfig.eingabedatei);

    // Process the read and write requests
    Request *requests = read_requests_from_file(numRequests, cacheConfig.eingabedatei);

    // Run the SystemC simulation
    printf("Starting the SystemC Simulation...\n");
    Result result = run_simulation(
        cacheConfig.cycles,
        cacheConfig.directmapped,
        cacheConfig.cachelines,
        cacheConfig.cachelineSize,
        cacheConfig.cacheLatency,
        cacheConfig.memoryLatency,
        numRequests,
        requests,
        cacheConfig.tracefile);

    // Print the result
    printf("\nSimulation Result:\n");
    printf(" - Cycles: %zu\n", result.cycles);
    printf(" - Cache misses: %zu\n", result.misses);
    printf(" - Cache hits: %zu\n", result.hits);
    printf(" - Primitive gate count: %zu\n", result.primitiveGateCount);

    // Free allocated space
    free(requests);

    return 0;
}

// Converts the given arguments to a CacheConfig struct
CacheConfig arguments_to_cacheConfig(int argc, char *argv[])
{
    // Set cache types to 0 to read users cache type input independently from the default cache config
    int defaultDirectmapped = cacheConfig.directmapped;
    int defaultFourway = cacheConfig.fourway;
    cacheConfig.directmapped = 0;
    cacheConfig.fourway = 0;

    // Loop depending on number of arguments (controlled using getopt_long)
    while (1)
    {
        const char *optstring = "c:dfs:l:a:m:t:h";
        static struct option longopts[] = {
            {"cycles", required_argument, NULL, 'c'},
            {"directmapped", no_argument, NULL, 'd'},
            {"fourway", no_argument, NULL, 'f'},
            {"cachelines", required_argument, NULL, 'l'},
            {"cacheline-size", required_argument, NULL, 's'},
            {"cache-latency", required_argument, NULL, 'a'},
            {"memory-latency", required_argument, NULL, 'm'},
            {"tf", required_argument, NULL, 't'},
            {"help", no_argument, NULL, 'h'},
            {0, 0, 0, 0} // End of arguments
        };

        int nextOption = getopt_long(argc, argv, optstring, longopts, NULL);

        if (nextOption == -1)
        {
            // No more arguments
            break;
        }

        // Processing each argument with getopt_long, while checking for invalid cases
        switch (nextOption)
        {
        case 'c':
            // Argument: Cycles, Expected: int > 0
            if (!is_integer(optarg) || atoi(optarg) <= 0)
            {
                print_help_and_exit_with_error("Error: Die Anzahl der Zyklen muss ein Integer sein und größer als 0\n");
            }
            cacheConfig.cycles = atoi(optarg);
            break;
        case 'd':
            // Argument: foruway, Expected: bool (Can't be used with directmapped)
            if (cacheConfig.fourway)
            {
                print_help_and_exit_with_error("Error: Es kann nur ein Cache Typ ausgewählt werden (directmapped oder fourway)\n");
            }
            cacheConfig.directmapped = 1;
            break;
        case 'f':
            // Argument: fourway, Expected: bool (Can't be used with directmapped)
            if (cacheConfig.directmapped)
            {
                print_help_and_exit_with_error("Error: Es kann nur ein Cache Typ ausgewählt werden (directmapped oder fourway)\n");
            }
            cacheConfig.fourway = 1;
            break;
        case 'l':
            // Argument: Cachelines, Expected: int > 0 and power of two
            if (!is_integer(optarg) || atoi(optarg) <= 0 || (atoi(optarg) & (atoi(optarg) - 1)) != 0)
            {
                print_help_and_exit_with_error("Error: Die Anzahl der Cachelines muss eine positive Zweierpotenz sein\n");
            }
            cacheConfig.cachelines = atoi(optarg);
            break;
        case 's':
            // Argument: Cacheline size, Expected: int >= 4
            if (!is_integer(optarg) || atoi(optarg) < 4)
            {
                print_help_and_exit_with_error("Error: Die Größe einer Cachezeile muss ein Integer sein und mindestes 4 (bytes)\n");
            }
            cacheConfig.cachelineSize = atoi(optarg);
            break;
        case 'a':
            // Argument: Cache Latency, Expected: int > 0
            if (!is_integer(optarg) || atoi(optarg) <= 0)
            {
                print_help_and_exit_with_error("Error: Die Cache Latency muss ein Integer sein und größer als 0\n");
            }
            cacheConfig.cacheLatency = atoi(optarg);
            break;
        case 'm':
            // Argument: Memory Latency, Expected: int > 0 && > cacheLatency
            if (!is_integer(optarg) || atoi(optarg) <= 0)
            {
                print_help_and_exit_with_error("Error: Die Memory Latency muss ein Integer sein und größer als 0 und die Cache Latency sein\n");
            }
            cacheConfig.memoryLatency = atoi(optarg);
            break;
        case 't':
            // Argument: Tracefile, Expected: char* with length < 255
            if (strlen(optarg) > 255)
            {
                print_help_and_exit_with_error("Error: Der Dateiname für das Tracefile darf maximal 255 Zeichen lang sein\n");
            }
            cacheConfig.tracefile = optarg;
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
    if (cacheConfig.directmapped == 0 && cacheConfig.fourway == 0)
    {
        cacheConfig.directmapped = defaultDirectmapped;
        cacheConfig.fourway = defaultFourway;
    }

    // If fourway cache, the number of cachelines must be a multiple of 4
    if (cacheConfig.fourway && cacheConfig.cachelines % 4 != 0)
    {
        print_help_and_exit_with_error("Error: Die Anzahl der Cachelines muss ein Vielfaches von 4 sein, wenn ein vierfach assoziativer Cache verwendet wird\n");
    }

    // The last argument is the Eingabedatei (independent from its argument position)
    if (optind >= argc)
    {
        // If there is no more argument, the Eingabedatei is missing
        print_help_and_exit_with_error("Error: Keine Eingabedatei angegeben\n");
    }
    else
    {
        if (access(argv[optind], F_OK) != 0)
        {
            // Check that the file with the given path exists
            print_help_and_exit_with_error("Error: Die Eingabedatei wurde nicht gefunden.\n");
        }
        cacheConfig.eingabedatei = argv[optind];
    }

    // All Invalid cases checked
    return cacheConfig;
}

// Print the given cache configuration
void print_cacheConfig(CacheConfig cacheConfig)
{
    printf("\nCache Configuration:\n");
    printf("  - Cycles: %d\n", cacheConfig.cycles);
    printf("  - DirectMapped/Fourway: %s\n", cacheConfig.directmapped ? "DirectMapped" : "Fourway");
    printf("  - Cachelines: %d\n", cacheConfig.cachelines);
    printf("  - Cacheline size: %d\n", cacheConfig.cachelineSize);
    printf("  - Cache latency: %d\n", cacheConfig.cacheLatency);
    printf("  - Memory latency: %d\n", cacheConfig.memoryLatency);
    printf("  - Tracefile: %s\n", cacheConfig.tracefile ? cacheConfig.tracefile : "None");
    printf("  - Eingabedatei: %s\n", cacheConfig.eingabedatei);
    printf("\n");
}

// Counts the lines in the given file
int count_lines_in_file(char *filename)
{
    FILE *file = fopen(filename, "r");
    if (file == NULL)
    {
        print_help_and_exit_with_error("Error: Fehler beim Öffnen der Datei: %s\n", filename);
    }

    int lines = 0;
    char line[256];
    while (fgets(line, sizeof(line), file))
    {
        lines++;
    }

    fclose(file);

    return lines;
}

// Reads the requests from the given .csv file
Request *read_requests_from_file(int numRequests, char *filename)
{
    // Allocate memory for the requests array
    Request *requests = malloc(numRequests * sizeof(Request));

    FILE *file = fopen(filename, "r");
    if (file == NULL)
    {
        print_help_and_exit_with_error("Error: Fehler beim Öffnen der Datei: %s\n", filename);
    }

    int index = 0;
    char line[MAX_REQUEST_LINE_LENGTH];
    while (fgets(line, sizeof(line), file))
    {
        char rw;
        unsigned int addr;
        unsigned int data = 0; // Initialize data to 0, as it might not be present for read requests

        // Remove any leading and trailing whitespace from the line
        char *trimmedLine = line;
        while (isspace((unsigned char)*trimmedLine))
        {
            trimmedLine++;
        }
        if (*trimmedLine == 0)
        {
            continue; // Skip empty lines
        }

        // Remove trailing whitespaces
        char *end = trimmedLine + strlen(trimmedLine) - 1;
        while (end > trimmedLine && isspace((unsigned char)*end))
        {
            end--;
        }
        end[1] = '\0';

        // Scan the format of the current line and store the entries in the local variables
        int numEntries = sscanf(trimmedLine, "%c , %x , %x", &rw, &addr, &data);

        // Check if the line is in the correct format
        if (numEntries == 2)
        {
            if (rw != 'R')
            {
                // read request expected for 2 entries
                print_help_and_exit_with_error("Error in Zeile %d: Bei einem Schreibzugriff muss ein Wert übergeben werden\n", index + 1);
            }
            else
            {
                // Read request
                requests[index].we = 0;
                requests[index].addr = addr;
                requests[index].data = 0; // No data for read requests
            }
        }
        else if (numEntries == 3)
        {
            if (rw != 'W')
            {
                // write request expected for 3 entries
                print_help_and_exit_with_error("Error in Zeile %d: Bei einem Lesezugriff darf kein Wert übergeben werden\n", index + 1);
            }
            else
            {
                // Write request
                requests[index].we = 1;
                requests[index].addr = addr;
                requests[index].data = data;
            }
        }
        else
        {
            // invalid line format
            print_help_and_exit_with_error("Error: Zeile %d ist nicht im Format <W/R, Adresse, Wert>: %s\n", index + 1, line);
        }

        index++;
    }

    fclose(file);

    return requests;
}

// Variadic function to print the error and help message and exits the program
void print_help_and_exit_with_error(char *errorMessage, ...)
{
    print_help();

    va_list args;
    va_start(args, errorMessage);

    fprintf(stderr, "Error: ");
    vfprintf(stderr, errorMessage, args);

    va_end(args);

    exit(1);
}

// Prints the description and usage of the arguments
void print_help()
{
    printf("Usage: cache_simulation [OPTIONS] <Eingabedatei>\n");
    printf("Options:\n");
    printf("  -c, --cycles <Zahl>         Die Anzahl der Zyklen, die simuliert werden sollen.\n");
    printf("  --directmapped              Simuliert einen direkt assoziativen Cache.\n");
    printf("  --fourway                   Simuliert einen vierfach assoziativen Cache.\n");
    printf("  --cachelines <Zahl>         Die Anzahl der Cachezeilen.\n");
    printf("  --cacheline-size <Zahl>     Die Größe einer Cachezeile in Byte.\n");
    printf("  --cache-latency <Zahl>      Die Latenzzeit eines Caches in Zyklen.\n");
    printf("  --memory-latency <Zahl>     Die Latenzzeit des Hauptspeichers in Zyklen.\n");
    printf("  --tf <Dateiname>            Ausgabedatei für ein Tracefile mit allen Signalen.\n");
    printf("  -h, --help                  Eine Beschreibung aller Optionen der Cachesimulation und Verwendung ausgeben und das Programm danach beendet.\n");
    printf("  Positional Arguments:\n");
    printf("  <Eingabedatei>              Die .csv Eingabedatei, die die zu verarbeitenden Daten enthält. Zeilenformat: <W, Adresse, Wert> oder <R, Adresse>\n");
    printf("\n");
}

// Checks if a string is an integer
int is_integer(char *str)
{
    while (*str != '\0')
    {
        if (!isdigit(*str))
        {
            return 0;
        }
        str++;
    }
    return 1;
}
