# Examples

.csv files to simulate cache requests.

## test.csv
cachelines=8

Requests:
- 1 - 8: Fill cache with new tags
- 9: Fill cacheline 1 with 1s
- 10: Fill cacheline 2 with 2s (Should write to cacheline 2 with offset 1 -> therefore use first byte of chacheline 3 aswell)
- 11: