#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>

void read_options(int argc, char *argv[]);
void print_option_variables();
void read_requests();
void print_help();
int isInteger(char* str);

int option_index = 0;

// TODO: Specify default values
int cycles = 0;
int directmapped = 0;
int fourway = 0;
int cacheline_size = 0;
int cachelines = 0;
int cache_latency = 0;
int memory_latency = 0;
char *tracefile = NULL;
char *eingabedatei = NULL;


int numRequests = 0;
const int MAX_REQUESTS = 1000;

struct Request requests[MAX_REQUESTS];
struct Request {
    uint32_t addr;
    uint32_t data;
    int we;
};

int main(int argc, char *argv[]) {
    // Populate the option variables
    read_options(argc, argv);

    // Feedback
    print_option_variables();

    // Process the read and write requests
    read_requests();

    // Main code (simulation start etc.)


    return 0;
}

void read_options(int argc, char *argv[]) {
    while (1) {
        static struct option parameter[] = {
            // Format: Name of the option, if a parameter needs to be passed, Flag if the option is used, intern reference
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

        const char* optstring = "c:dfs:l:a:m:t:h";
        // getopt_long: Bearbeitet alle Optionen mit - oder -- und speichert den Parameter als String in optarg
        int c = getopt_long(argc, argv, optstring, parameter, &option_index);

        if (c == -1)
			// No more arguments
            break;

        // Check for invalid combinations
        //     cycles:  > 0
        //     directmapped:  fourway = 0
        //     fourway:  directmapped = 0
        //     cacheline_size:  > 0
        //     cachelines:  > 0
        //     cache_latency:  > 0
        //     memory_latency:  > 0 && > cache_latency
        //     tracefile:  optional, length < 255
        //     eingabedatei:  valid path
        switch (c) {
            case 'c':
                if (!isInteger(optarg) || atoi(optarg) <= 0) {
                    fprintf(stderr, "Error: Die Anzahl der Zyklen muss ein Integer sein und größer als 0\n");
                    print_help();
                    exit(1);
                }
                cycles = atoi(optarg);
                break;
            case 'd':
                if (fourway) {
                    fprintf(stderr, "Error: Es kann nur ein Cache Typ ausgewählt werden (directmapped oder fourway)\n");
                    print_help();
                    exit(1);
                }
                directmapped = 1;
                break;
            case 'f':
                if (directmapped) {
                    fprintf(stderr, "Error: Es kann nur ein Cache Typ ausgewählt werden (directmapped oder fourway)\n");
                    print_help();
                    exit(1);
                }
                fourway = 1;
                break;
            case 's':
                if (!isInteger(optarg) || atoi(optarg) <= 0) {
                    fprintf(stderr, "Error: Die Größe einer Cachezeile muss ein Integer sein und größer als 0\n");
                    print_help();
                    exit(1);
                }
                cacheline_size = atoi(optarg);
                break;
            case 'l':
                if (!isInteger(optarg) || atoi(optarg) <= 0) {
                    fprintf(stderr, "Error: Die Anzahl der Cachelines muss ein Integer sein und größer als 0\n");
                    print_help();
                    exit(1);
                }
                cachelines = atoi(optarg);
                break;
            case 'a':
                if (!isInteger(optarg) || atoi(optarg) <= 0) {
                    fprintf(stderr, "Error: Die Cache Latency muss ein Integer sein und größer als 0\n");
                    print_help();
                    exit(1);
                }
                cache_latency = atoi(optarg);
                break;
            case 'm':
                if (!isInteger(optarg) || atoi(optarg) <= 0) {
                    fprintf(stderr, "Error: Die Memory Latency muss ein Integer sein und größer als 0 und die Cache Latency sein\n");
                    print_help();
                    exit(1);
                }
                memory_latency = atoi(optarg);
                break;
            case 't':
                if (strlen(optarg) > 255) {
                    fprintf(stderr, "Error: Der Dateiname für das Tracefile darf maximal 255 Zeichen lang sein\n");
                    print_help();
                    exit(1);
                }
                tracefile = optarg;
                break;
            case 'h':
                print_help();
                exit(0);
            case '?':
            default:
                printf("Something went wrong\n");
                print_help();
                exit(1);
        }
    }

    if (optind < argc) {
        eingabedatei = argv[optind];
        if (access(eingabedatei, F_OK) != 0) {
            fprintf(stderr, "Error: Die Eingabedatei wurde nicht gefunden\n");
            print_help();
            exit(1);
        }
    } else {
        fprintf(stderr, "Error: Keine Eingabedatei angegeben\n");
        print_help();
        exit(1);
    }
}

void print_option_variables() {
    printf("Cycles: %d\n", cycles);
    printf("Direct mapped: %d\n", directmapped);
    printf("Four way: %d\n", fourway);
    printf("Cacheline size: %d\n", cacheline_size);
    printf("Cachelines: %d\n", cachelines);
    printf("Cache latency: %d\n", cache_latency);
    printf("Memory latency: %d\n", memory_latency);
    printf("Tracefile: %s\n", tracefile ? tracefile : "None");
    printf("Eingabedatei: %s\n", eingabedatei);
}

void read_requests() {
    // Read the requests from the file
    // Format: <R/W>,<address_hexordec>,<value_hexordec>
    // Error: If W, value has to be present, if R field value has to be empty

    FILE *file = fopen(eingabedatei, "r");
    if (file == NULL) {
        fprintf(stderr, "Error: Fehler beim Öffnen der Datei: %s\n",  eingabedatei);
        return;
    }

    char line[256];
    while (fgets(line, sizeof(line), file)) {
        if (numRequests >= MAX_REQUESTS) {
            fprintf(stderr, "Error: Zu viele Requests, die ersten %d wurden verarbeitet.\n", MAX_REQUESTS);
            break;
        }

        char rw;
        if (sscanf(line, "%c,%x,%x", &rw, &requests[numRequests].addr, &requests[numRequests].data) == 3) {
            requests[numRequests].we = (rw == 'W') ? 1 : 0;
            if (rw == 'R' && requests[numRequests].data != 0) {
                fprintf(stderr, "Error: In Zeile %d: Bei einem Lesezugriff darf kein Wert übergeben werden\n", numRequests + 1);
                return;
            }
            numRequests++;
        } else {
            fprintf(stderr, "Error: Zeile %d ist nicht im Format <W/R, Adresse, Wert>: %s\n", numRequests + 1, line);
            return;
        }
    }

    fclose(file);

    printf("Done reading %d requests.\n", numRequests); 
}

void print_help() {
    printf("\nUsage: program [OPTIONS] <Dateiname>\n");
    printf("Options:\n");
    printf("  -c, --cycles <Zahl>         Die Anzahl der Zyklen, die simuliert werden sollen.\n");
    printf("  --directmapped              Simuliert einen direkt assoziativen Cache.\n");
    printf("  --fourway                   Simuliert einen vierfach assoziativen Cache.\n");
    printf("  --cacheline-size <Zahl>     Die Größe einer Cachezeile in Byte.\n");
    printf("  --cachelines <Zahl>         Die Anzahl der Cachezeilen.\n");
    printf("  --cache-latency <Zahl>      Die Latenzzeit eines Caches in Zyklen.\n");
    printf("  --memory-latency <Zahl>     Die Latenzzeit des Hauptspeichers in Zyklen.\n");
    printf("  --tf <Dateiname>            Ausgabedatei für ein Tracefile mit allen Signalen.\n");
    printf("  -h, --help                  Eine Beschreibung aller Optionen des Programms und Verwendung ausgeben und das Programm danach beendet.\n");
}

// Helper function
int isInteger(char* str) {
    while (*str != '\0') {
        if (!isdigit(*str)) {
            return 0;
        }
        str++;
    }
    return 1;
}

/*
    Beispiel:
    ./program -c 1000 --directmapped --cacheline-size 64 --cachelines 16 --cache-latency 2 --memory-latency 10 --tf tracefile.vcd requests.csv
*/ 