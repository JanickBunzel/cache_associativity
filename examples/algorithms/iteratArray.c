#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define BASE_ADDRESS 0x10000000 // Base address for virtual addresses

// Function to log access in a CSV file
void logAccess(char accessType, uint32_t virtualAddress, uint32_t value, FILE *logFile) {
    if (accessType == 'W') {
        fprintf(logFile, "W,0x%08x,0x%08x\n", virtualAddress, value);
    } else if (accessType == 'R') {
        fprintf(logFile, "R,0x%08x,\n", virtualAddress);
    }
}

// Function to convert physical address to virtual address
uint32_t toVirtualAddress(void *address, void *base) {
    return (uint32_t)((uintptr_t)address - (uintptr_t)base + BASE_ADDRESS);
}

// Function to iterate through an array and log the accesses
void writeArray(uint32_t *array, int size, void *baseAddress, FILE *logFile) {
    for (int i = 0; i < size; ++i) {
        array[i] = i + 1;
        logAccess('W', toVirtualAddress(&array[i], baseAddress), array[i], logFile);
    }
}

// Function to iterate back through an array and log read accesses
void readArrayBackwards(uint32_t *array, int size, void *baseAddress, FILE *logFile) {
    for (int i = size - 1; i >= 0; --i) {
        uint32_t value = array[i];
        logAccess('R', toVirtualAddress(&array[i], baseAddress), value, logFile);
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <size>\n", argv[0]);
        return 1;
    }

    int size = atoi(argv[1]);

    // Open the log file in write mode to replace its content if it already exists
    FILE *logFile = fopen("/Users/julian/Documents/Studium/ss24/gra/workspace/cache_associativity/examples/array_access_log.csv", "w");
    if (logFile == NULL) {
        fprintf(stderr, "Error opening log file\n");
        return 1;
    }

    // Allocate memory for the array
    uint32_t *array = (uint32_t *)malloc(size * sizeof(uint32_t));
    if (array == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        fclose(logFile);
        return 1;
    }

    // Base address for converting virtual addresses
    void *baseAddress = array;

    // Write to the array
    writeArray(array, size, baseAddress, logFile);

    // Read the array backwards
    readArrayBackwards(array, size, baseAddress, logFile);

    // Free allocated memory and close the log file
    free(array);
    fclose(logFile);

    return 0;
}

// Command to compile: gcc -o array_iteration iteratArray.c
// Command to run: ./array_iteration 300
