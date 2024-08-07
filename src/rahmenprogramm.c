#include <ctype.h>
#include <getopt.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "request.h"
#include "result.h"

// Struct to store properties of the cache
typedef struct CacheConfig
{
    int cycles;         // Number of cycles to simulate
    int directmapped;   // bool
    int fourway;        // bool
    int cachelines;     // Number of cachelines
    int cachelineSize;  // In bytes
    int cacheLatency;   // In cycles
    int memoryLatency;  // In cycles
    char *tracefile;    // Path to a file
    char *eingabedatei; // Path to the input file
} CacheConfig;

// Default cache configuration
CacheConfig cacheConfig = {
    .cycles = 34000,      // With a hit rate of 70%, therefore 17 cycles/request on average, this covers roughly 2000 requests
    .directmapped = 1,    // Using direct mapped cache as default
    .fourway = 0,         // Using direct mapped cache as default
    .cachelines = 512,    // With 512 cachelines of 32 bytes, this provides an 16KB cache, typical size of a L1 cache
    .cachelineSize = 32,  // Standard size that balances spatial locality and overhead
    .cacheLatency = 4,    // Quick access for first-level cache
    .memoryLatency = 100, // Slower access for main memory
    .tracefile = "",      // By default, no tracefile is generated
    .eingabedatei = NULL, // Input file needs to be provided
};

// Flags to check if the values are passed by the user
CacheConfig configPassed = {
    .cycles = 0,
    .directmapped = 0,
    .fourway = 0,
    .cachelines = 0,
    .cachelineSize = 0,
    .cacheLatency = 0,
    .memoryLatency = 0,
    .tracefile = NULL,
    .eingabedatei = NULL,
};
int configPassed_printsLevel = 0;

// Macro to handle request lines in the input file
int MAX_REQUEST_LINE_LENGTH = 256;

// Global variable to enable additional debug prints, controlled by the -p argument and disabled by default
int printsLevel = 0;
char *highlightColor = "\033[0;36m";
char *highlightDataColor = "\033[0;32m";
char *resetColor = "\033[0m";

// Function declarations
CacheConfig arguments_to_cacheConfig(int argc, char *argv[]);
void print_cacheConfig(CacheConfig cacheConfig);
int count_requests_in_file(char *filename);
Request *read_requests_from_file(int numRequests, char *filename);
void print_help_and_exit_with_error(char *errorMessage, ...);
void print_help();
void print_simulation_result(CacheConfig cacheConfig, Result result, int numRequests);
int is_integer(char *str);
unsigned parse_value(char *value);

// Declaration of the extern C++ function
#ifdef __cplusplus
extern "C"
{
#endif

    Result run_simulation(
        int cycles,
        int directMapped,
        unsigned cachelines,
        unsigned cachelineSize,
        unsigned cacheLatency,
        unsigned memoryLatency,
        size_t numRequests,
        Request requests[],
        const char *tracefile);

#ifdef __cplusplus
}
#endif

// Main function, entry point of the program
int main(int argc, char *argv[])
{
    // Process the given parameters
    CacheConfig cacheConfig = arguments_to_cacheConfig(argc, argv);

    // Count the requests
    int numRequests = count_requests_in_file(cacheConfig.eingabedatei);

    // Process the read and write requests
    Request *requests = read_requests_from_file(numRequests, cacheConfig.eingabedatei);

    // Summary of the cache configuration
    print_cacheConfig(cacheConfig);

    // Run the SystemC simulation
    printf("Starting the SystemC Simulation...\n");
    Result result = run_simulation(cacheConfig.cycles, cacheConfig.directmapped, cacheConfig.cachelines, cacheConfig.cachelineSize, cacheConfig.cacheLatency, cacheConfig.memoryLatency, numRequests, requests, cacheConfig.tracefile);

    // Print the result
    print_simulation_result(cacheConfig, result, numRequests);

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

    // Parameters for getopt_long
    const char *optstring = "c:dfp:l:s:a:m:t:h";
    static struct option longopts[] = {
        {"cycles", required_argument, NULL, 'c'},
        {"directmapped", no_argument, NULL, 'd'},
        {"fourway", no_argument, NULL, 'f'},
        {"verbose-prints", required_argument, NULL, 'p'},
        {"cachelines", required_argument, NULL, 'l'},
        {"cacheline-size", required_argument, NULL, 's'},
        {"cache-latency", required_argument, NULL, 'a'},
        {"memory-latency", required_argument, NULL, 'm'},
        {"tf", required_argument, NULL, 't'},
        {"help", no_argument, NULL, 'h'},
        {0, 0, 0, 0} // End of arguments
    };

    // Check for invalid short options that might be misinterpreted as long options
    for (int i = 1; i < argc; i++)
    {
        if (argv[i][0] == '-' && argv[i][1] != '-' && strlen(argv[i]) > 2)
        {
            print_help_and_exit_with_error("Error: Unbekanntes Argument %s\n", argv[i]);
        }
    }

    // Loop depending on number of arguments (controlled using getopt_long)
    while (1)
    {
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
            // Check if the user has already set the cycles
            if (configPassed.cycles)
            {
                print_help_and_exit_with_error("Error: Die Anzahl der Zyklen wurde mehrmals übergeben\n");
            }
            configPassed.cycles = 1;

            // Argument: Cycles, Expected: int > 0
            if (!is_integer(optarg) || atoi(optarg) <= 0)
            {
                print_help_and_exit_with_error("Error: Die Anzahl der Zyklen muss ein Integer sein und größer als 0\n");
            }
            cacheConfig.cycles = atoi(optarg);
            break;
        case 'd':
            // Argument: foruway, Expected: bool (Can't be used with directmapped or set twice)
            if (cacheConfig.fourway || configPassed.directmapped || configPassed.fourway)
            {
                print_help_and_exit_with_error("Error: Es kann nur ein Cache Typ ausgewählt werden (directmapped oder fourway)\n");
            }
            configPassed.directmapped = 1;

            cacheConfig.directmapped = 1;
            break;
        case 'f':
            // Argument: fourway, Expected: bool (Can't be used with directmapped or set twice)
            if (cacheConfig.directmapped || configPassed.fourway || configPassed.directmapped)
            {
                print_help_and_exit_with_error("Error: Es kann nur ein Cache Typ ausgewählt werden (directmapped oder fourway)\n");
            }
            configPassed.fourway = 1;

            cacheConfig.fourway = 1;
            break;
        case 'p':
            // Check if the user has already set the prints level
            if (configPassed_printsLevel)
            {
                print_help_and_exit_with_error("Error: Der Wert für verbose-prints wurde mehrmals übergeben\n");
            }
            configPassed_printsLevel = 1;

            // Argument: Verbose prints, Expected: 3 >= int >= 0
            if (!is_integer(optarg) || atoi(optarg) < 0 || atoi(optarg) > 3)
            {
                print_help_and_exit_with_error("Error: Der Wert für verbose-prints muss ein Integer sein und zwischen 0 und 3 liegen\n");
            }
            printsLevel = atoi(optarg);
            break;
        case 'l':
            if (configPassed.cachelines)
            {
                print_help_and_exit_with_error("Error: Die Anzahl der Cachelines wurde mehrmals übergeben\n");
            }
            configPassed.cachelines = 1;

            // Argument: Cachelines, Expected: int > 0 and power of two
            if (!is_integer(optarg) || atoi(optarg) <= 0 || (atoi(optarg) & (atoi(optarg) - 1)) != 0)
            {
                print_help_and_exit_with_error("Error: Die Anzahl der Cachelines muss eine Zweierpotenz größer 0 sein\n");
            }
            cacheConfig.cachelines = atoi(optarg);
            break;
        case 's':
            // Check if the user has already set the cacheline size
            if (configPassed.cachelineSize)
            {
                print_help_and_exit_with_error("Error: Die Größe einer Cachezeile wurde mehrmals übergeben\n");
            }
            configPassed.cachelineSize = 1;

            // Argument: Cacheline size, Expected: int >= 4
            if (!is_integer(optarg) || atoi(optarg) < 4)
            {
                print_help_and_exit_with_error("Error: Die Größe einer Cachezeile muss ein Integer sein und mindestes 4 (bytes)\n");
            }
            cacheConfig.cachelineSize = atoi(optarg);
            break;
        case 'a':
            // Check if the user has already set the cache latency
            if (configPassed.cacheLatency)
            {
                print_help_and_exit_with_error("Error: Die Cache Latency wurde mehrmals übergeben\n");
            }
            configPassed.cacheLatency = 1;

            // Argument: Cache Latency, Expected: int > 0
            if (!is_integer(optarg) || atoi(optarg) <= 0)
            {
                print_help_and_exit_with_error("Error: Die Cache Latency muss ein Integer sein und größer als 0\n");
            }
            cacheConfig.cacheLatency = atoi(optarg);
            break;
        case 'm':
            // Check if the user has already set the memory latency
            if (configPassed.memoryLatency)
            {
                print_help_and_exit_with_error("Error: Die Memory Latency wurde mehrmals übergeben\n");
            }
            configPassed.memoryLatency = 1;

            // Argument: Memory Latency, Expected: int > 0 && > cacheLatency
            if (!is_integer(optarg) || atoi(optarg) <= 0)
            {
                print_help_and_exit_with_error("Error: Die Memory Latency muss ein Integer sein und größer als 0 und die Cache Latency sein\n");
            }
            cacheConfig.memoryLatency = atoi(optarg);
            break;
        case 't':
            // Check if the user has already set the tracefile
            if (configPassed.tracefile)
            {
                print_help_and_exit_with_error("Error: Der Dateiname für das Tracefile wurde mehrmals übergeben\n");
            }
            configPassed.tracefile = "1";

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

    // Check for any extra positional arguments
    if (optind < argc - 1)
    {
        print_help_and_exit_with_error("Error: Zu viele Positionsargumente angegeben. (Wurde die Eingabedatei mehrmals gesetzt?)\n");
    }

    // All Invalid cases checked
    return cacheConfig;
}

// Print the given cache configuration
void print_cacheConfig(CacheConfig cacheConfig)
{
    if (printsLevel >= 1)
    {
        printf("%s", highlightColor); // Start highlighted color for the cache configuration
    }

    printf("\nCache Configuration:\n");
    printf("  - Cycles: %d\n", cacheConfig.cycles);
    // Print the cache type pink if additional debug prints are enabled
    if (printsLevel >= 1)
    {
        printf("  - DirectMapped/Fourway: \033[0;95m%s\n%s", cacheConfig.directmapped ? "DirectMapped" : "Fourway", highlightColor);
    }
    else
    {
        printf("  - DirectMapped/Fourway: %s\n", cacheConfig.directmapped ? "DirectMapped" : "Fourway");
    }
    printf("  - Cachelines: %d\n", cacheConfig.cachelines);
    printf("  - Cacheline size: %d\n", cacheConfig.cachelineSize);
    printf("  - Cache latency: %d\n", cacheConfig.cacheLatency);
    printf("  - Memory latency: %d\n", cacheConfig.memoryLatency);
    printf("  - Tracefile: %s\n", cacheConfig.tracefile ? cacheConfig.tracefile : "None");
    printf("  - Eingabedatei: %s\n", cacheConfig.eingabedatei);
    printf("\n");

    if (printsLevel >= 1)
    {
        printf("%s", resetColor); // Reset color
    }
}

// Counts the requests in the given file, throwing an error when encountering a blank line in between requests. Format: <W/R, Address, Value>
int count_requests_in_file(char *filename)
{
    FILE *file = fopen(filename, "r");
    if (file == NULL)
    {
        print_help_and_exit_with_error("Error: Fehler beim Öffnen der Datei: %s\n", filename);
    }

    int numRequests = 0;
    int currentLineNumber = 0;
    int firstBlankLineNumber = -1;
    int lastBlankLineNumber = -1;
    char line[256];
    while (fgets(line, sizeof(line), file))
    {
        currentLineNumber++;

        // Remove leading whitespace
        char *trimmedLine = line;
        while (isspace((unsigned char)*trimmedLine))
        {
            trimmedLine++;
        }

        // Check if the line is empty (excluding trailing blank lines)
        if (*trimmedLine == '\0' && !feof(file))
        {
            // Store empty line number
            if (firstBlankLineNumber == -1)
            {
                firstBlankLineNumber = currentLineNumber;
            }
            else if (lastBlankLineNumber != -1)
            {
                lastBlankLineNumber = currentLineNumber;
            }
        }
        else
        {
            // If a non-empty line comes after a blank line, throw an error because blank lines are not allowed between requests
            if (firstBlankLineNumber != -1)
            {
                // Blank line exists between requests, throw an error for either single or multiple blank lines
                if (lastBlankLineNumber == -1 || firstBlankLineNumber == lastBlankLineNumber)
                {
                    // Single blank line in between requests
                    print_help_and_exit_with_error("Error: Zeile %d ist nicht im Format <W/R, Adresse, Wert> (Leere Zeilen dürfen nicht zwischen den Requests stehen)\n", firstBlankLineNumber);
                }
                else
                {
                    // Multiple blank lines in between requests
                    print_help_and_exit_with_error("Error: Zeilen %d bis %d sind nicht im Format <W/R, Adresse, Wert> (Leere Zeilen dürfen nicht zwischen den Requests stehen)\n", firstBlankLineNumber, lastBlankLineNumber);
                }
            }
        }

        // Count non-empty lines
        if (*trimmedLine != '\0')
        {
            numRequests++;
        }
    }

    fclose(file);

    return numRequests;
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
        char addr_str[32];
        char data_str[32] = "0"; // Initialize data to "0", as it might not be present for read requests

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
        int numEntries = sscanf(trimmedLine, "%c , %31[^,] , %31s", &rw, addr_str, data_str);

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
                requests[index].addr = parse_value(addr_str);
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
                requests[index].addr = parse_value(addr_str);
                requests[index].data = parse_value(data_str);
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
    va_list args;
    va_start(args, errorMessage);

    fprintf(stderr, "\033[1;31m");
    vfprintf(stderr, errorMessage, args);
    fprintf(stderr, "\033[0m");

    va_end(args);

    print_help();

    exit(1);
}

// Prints the description and usage of the arguments
void print_help()
{
    printf("Usage: cache_simulation [OPTIONS] <Eingabedatei>\n");
    printf("Options:\n");
    printf("  -c, --cycles <Zahl>           Die Anzahl der Zyklen, die simuliert werden sollen.\n");
    printf("  --directmapped                Simuliert einen direkt assoziativen Cache.\n");
    printf("  --fourway                     Simuliert einen vierfach assoziativen Cache.\n");
    printf("  --cachelines <Zahl>           Die Anzahl der Cachezeilen.\n");
    printf("  --cacheline-size <Zahl>       Die Größe einer Cachezeile in Byte.\n");
    printf("  --cache-latency <Zahl>        Die Latenzzeit eines Caches in Zyklen.\n");
    printf("  --memory-latency <Zahl>       Die Latenzzeit des Hauptspeichers in Zyklen.\n");
    printf("  --tf <Dateiname>              Ausgabedatei für ein Tracefile mit allen Signalen.\n");
    printf("  -h, --help                    Eine Beschreibung aller Optionen der Cachesimulation und Verwendung ausgeben und das Programm danach beendet.\n");
    printf("  <Eingabedatei>                Positional Argument mit der .csv Eingabedatei, die die zu verarbeitenden Daten enthält. Zeilenformat: <W, Adresse, Wert> oder <R, Adresse>\n");
    printf("\n");
    printf("  -p, --verbose-prints <Zahl>   Aktiviert zusätzliches Feedback der Simulation. 3 Level für ausführliche Debug Informationen:\n");
    printf("                                    - 0: Standardrückgabe\n");
    printf("                                    - 1: + Cache States\n");
    printf("                                    - 2: + Memory States & Cachline Misses/Hits\n");
    printf("                                    - 3: + Cycles Count\n");
    printf("\n");
}

// Prints a summary of the simulation
void print_simulation_result(CacheConfig cacheConfig, Result result, int numRequests)
{
    if (printsLevel >= 1)
    {
        printf("%s", highlightColor); // Start highlighted color for the summary of the simulation
    }

    // Print the standard simulation result
    printf("\nSimulation Result:\n");
    printf(" - Cycles: %zu\n", result.cycles);
    printf(" - Cache misses: %zu\n", result.misses);
    printf(" - Cache hits: %zu\n", result.hits);
    printf(" - Primitive gate count: %zu\n", result.primitiveGateCount);

    if (printsLevel >= 1)
    {
        // Print additional debug info
        printf("\nAdditional Debug Info:");

        double hitRate = (double)result.hits / (result.hits + result.misses);
        double missRate = (double)result.misses / (result.hits + result.misses);
        double avgCyclesPerRequest = (double)result.cycles / numRequests;
        printf("\n - Hit Rate: %.2f%%", hitRate * 100);
        printf("\n - Miss Rate: %.2f%%", missRate * 100);
        printf("\n - Average Cycles per Request: %.2f\n", avgCyclesPerRequest);

        // Print the cacheConfig again because of large amount of debug info was printed
        print_cacheConfig(cacheConfig);

        printf("%s", resetColor); // Reset color
    }
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

// Parses the value from the given string, supporting both decimal and hexadecimal values depending on the prefix "" or "0x"/"0X"
unsigned parse_value(char *value)
{
    if (strstr(value, "0x") != NULL || strstr(value, "0X") != NULL)
    {
        // Hexadecimal value
        return strtoul(value, NULL, 16);
    }
    else
    {
        // Decimal value
        return strtoul(value, NULL, 10);
    }
}
