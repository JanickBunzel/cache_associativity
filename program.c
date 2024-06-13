#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>

void print_help() {
    printf("Usage: program [OPTIONS] <Dateiname>\n");
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

int main(int argc, char *argv[]) {
    int option_index = 0;
    int cycles = 0;
    int cacheline_size = 0;
    int cachelines = 0;
    int cache_latency = 0;
    int memory_latency = 0;
    char *tracefile = NULL;
    int directmapped = 0;
    int fourway = 0;

    while (1) {
        static struct option parameter[] = {
            // Name of the option, if a parameter needs to be passed, Flag if the option is used, intern reference
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
        // Bearbeitet alle Optionen mit - oder -- und speichert den Parameter als String in optarg
        int c = getopt_long(argc, argv, optstring, parameter, &option_index);

        if (c == -1)
			// No more arguments
            break;

        switch (c) {
            case 'c':
                cycles = atoi(optarg);
                break;
            case 'd':
                directmapped = 1;
                break;
            case 'f':
                fourway = 1;
                break;
            case 's':
                cacheline_size = atoi(optarg);
                break;
            case 'l':
                cachelines = atoi(optarg);
                break;
            case 'a':
                cache_latency = atoi(optarg);
                break;
            case 'm':
                memory_latency = atoi(optarg);
                break;
            case 't':
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
        char *filename = argv[optind];
        printf("Processing file: %s\n", filename);
    } else {
        fprintf(stderr, "Error: No input file specified\n");
        print_help();
        exit(1);
    }

    // Debug Feddback
    printf("Cycles: %d\n", cycles);
    printf("Direct mapped: %d\n", directmapped);
    printf("Four way: %d\n", fourway);
    printf("Cacheline size: %d\n", cacheline_size);
    printf("Cachelines: %d\n", cachelines);
    printf("Cache latency: %d\n", cache_latency);
    printf("Memory latency: %d\n", memory_latency);
    printf("Tracefile: %s\n", tracefile ? tracefile : "None");

    // Check for invalid combinations
    if (directmapped && fourway) {
        fprintf(stderr, "Error: Direct mapped and four way cache cannot be used together\n");
        exit(1);
    }
    // TODO
    // ...
    

    // Main code (simulation start etc.)

    return 0;
}

/*
    Beispiel:
    ./program -c 1000 --directmapped --cacheline-size 64 --cachelines 16 --cache-latency 2 --memory-latency 10 --tf tracefile.vcd requests.csv
*/ 