#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define BASE_ADDRESS 0x10000000 // Basisadresse für virtuelle Adressen

// Funktion zur Protokollierung in eine CSV-Datei
void logAccess(char accessType, uint32_t virtualAddress, uint32_t value) {
    FILE *logFile = fopen("/Users/julian/Documents/Studium/ss24/gra/workspace/cache_associativity/examples/memory_access_log.csv", "a");
    if (logFile == NULL) {
        fprintf(stderr, "Error opening log file\n");
        exit(1);
    }
    if (accessType == 'W') {
        fprintf(logFile, "W,0x%08x,0x%08x\n", virtualAddress, value);
    } else if (accessType == 'R') {
        fprintf(logFile, "R,0x%08x,\n", virtualAddress);
    }
    fclose(logFile);
}

// Funktion zur Umrechnung der physischen Adresse in eine virtuelle Adresse
uint32_t toVirtualAddress(void *address, void *base) {
    return (uint32_t)((uintptr_t)address - (uintptr_t)base + BASE_ADDRESS);
}

// Funktion zur Matrixmultiplikation mit Protokollierung
void multiplyMatrices(uint32_t *firstMatrix, uint32_t *secondMatrix, uint32_t *resultMatrix, int n, void *baseAddress) {
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            resultMatrix[i * n + j] = 0;
            logAccess('W', toVirtualAddress(&resultMatrix[i * n + j], baseAddress), resultMatrix[i * n + j]);
            for (int k = 0; k < n; ++k) {
                uint32_t readFirst = firstMatrix[i * n + k];
                uint32_t readSecond = secondMatrix[k * n + j];
                logAccess('R', toVirtualAddress(&firstMatrix[i * n + k], baseAddress), readFirst);
                logAccess('R', toVirtualAddress(&secondMatrix[k * n + j], baseAddress), readSecond);
                resultMatrix[i * n + j] += readFirst * readSecond;
                logAccess('W', toVirtualAddress(&resultMatrix[i * n + j], baseAddress), resultMatrix[i * n + j]);
            }
        }
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <n>\n", argv[0]);
        return 1;
    }

    int n = atoi(argv[1]);

    // Speicher für Matrizen reservieren
    uint32_t *firstMatrix;
    uint32_t *secondMatrix;
    uint32_t *resultMatrix;

    if (posix_memalign((void**)&firstMatrix, 32, n * n * sizeof(uint32_t)) != 0 ||
        posix_memalign((void**)&secondMatrix, 32, n * n * sizeof(uint32_t)) != 0 ||
        posix_memalign((void**)&resultMatrix, 32, n * n * sizeof(uint32_t)) != 0) {
        fprintf(stderr, "Memory allocation failed\n");
        return 1;
    }

    // Basisadresse für die Umrechnung der virtuellen Adressen
    void *baseAddress = firstMatrix < secondMatrix ? (firstMatrix < resultMatrix ? firstMatrix : resultMatrix) : (secondMatrix < resultMatrix ? secondMatrix : resultMatrix);

    // Beispielhafte Initialisierung der Matrizen
    for (int i = 0; i < n * n; ++i) {
        firstMatrix[i] = i + 1;  // Einfacher Wert, um die Matrix zu füllen
        logAccess('W', toVirtualAddress(&firstMatrix[i], baseAddress), firstMatrix[i]);
    }

    for (int i = 0; i < n * n; ++i) {
        secondMatrix[i] = i + 1;  // Einfacher Wert, um die Matrix zu füllen
        logAccess('W', toVirtualAddress(&secondMatrix[i], baseAddress), secondMatrix[i]);
    }

    // Matrizen multiplizieren
    multiplyMatrices(firstMatrix, secondMatrix, resultMatrix, n, baseAddress);

    // Speicher freigeben
    free(firstMatrix);
    free(secondMatrix);
    free(resultMatrix);

    return 0;
}

//Command to compile : gcc -o matrix_multiplication main.cpp
//Command to run : ./matrix_multiplication 300