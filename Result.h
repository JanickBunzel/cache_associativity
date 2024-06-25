#ifndef RESULT_H
#define RESULT_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h> // For size_t

// Struct to store results
typedef struct Result {
    int cycles;
    int misses;
    int hits;
    int primitiveGateCount;
} Result;

#ifdef __cplusplus
}
#endif

#endif // RESULT_H