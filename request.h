#ifndef REQUEST_H
#define REQUEST_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h> // For uint32_t

    // Struct to define cache requests
    typedef struct request
    {
        uint32_t addr;
        uint32_t data;
        int we;
    } Request;

#ifdef __cplusplus
}
#endif

#endif // REQUEST_H