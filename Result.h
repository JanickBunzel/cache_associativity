#ifndef RESULT_H
#define RESULT_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h> // For size_t

// Struct to store results
typedef struct Result {
    size_t cycles;
    size_t misses;
    size_t hits;
    size_t primitiveGateCount;
} Result;

#ifdef __cplusplus
}
#endif

#endif // RESULT_H