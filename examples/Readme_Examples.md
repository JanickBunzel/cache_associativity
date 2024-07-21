# Examples

In this directory you can find examples for request patterns of a cache.
We have written scripts to automatically generate them but also created validation request patterns manually to verify the programs correctness.

The [CachePerformance.pdf](./CachePerformance.pdf) contains a table of some results of our runs with the cache_simulation

## Overview:
- [algorithms](#algorithms)
- [betterDirectmapped](#betterDirectmapped)
- [betterFourway](#betterFourway)
- [validation](#validation)

---

### `/algorithms`
Scripts to generate request patterns:
- algorithms.py: Simulates a Quicksort and an Iteration over a Customizable Array with n elements.
- MatrixMultiplication.java: Simulates a Matrix Multiplication with two matrices of size n x n.

Examples:
- iterate_n.csv: Iterates over an array of size n.
- matrix_multiplication_n.csv: Multiplies two matrices of size n x n.
- quicksort_n.csv: Sorts an array of size n with Quicksort


---
### `/betterDirectmapped`
Scripts to generate request patterns:
- betterDirectmapped.py: Generates a linear request pattern (5000)

Examples:
- betterDirectmapped.csv: Shows that the fourway cache is not more efficient than the directmapped cache in this case, but needs a more complex architecture (primitiveGateCount).


---
### `/betterFourway`
Scripts to generate request patterns:
- extreme.py: Generates a request pattern that requests the same 4 addresses in a loop. (4 Writes, 4 Reads)
- extreme2.py: Generates a request pattern that requests the same 5 addresses in a loop. (Alternating 5 Writes, 5 Reads and 4 Writes, 4 Reads)

Examples:
- extreme.csv: Shows that the request pattern exploits the full potential of a fourway cache because the directmapped cache maps all addresses to the same cache line however the fourway cache has a set of 4 cachelines to choose from when mapping one address. This pattern hits 4 out of 4 times. (Excluding cold misses at the start)
- extreme2.csv: Similar to the above, but slightly less optimized for the fourway cache. The ratio of requests with the same address compared to the amount of cachelines in a set is not as optimal as in extreme.csv, in fact, it hits 4 out of 9 times.


---
### `/validation`
These are manually created request patterns to verify the correctness of the cache.
Each .csv request pattern has a corresponding .md file with the expected output explaining the expected state of the cache.
