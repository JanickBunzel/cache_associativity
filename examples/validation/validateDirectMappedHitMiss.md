# Check if cache handles hit / miss

### cache data:
- cachline-size = 16 Byte
- cachesize = 16
- cache latency = 10 cycles
- memory latency = 40 cycles

### bits
- offset = 4
- index = 4
- tag = 24

### requests
1) test if write and read miss is detected correctly and if cycles have right amount: one write miss and two read misses because of different tags but same index. 
````
W,0xF0000000,0xabababab
R,0x0F000000,
R,0x00F00000,
````
- Cycles = 3 * cache latency (accessing the cache) + 3  * memory latency (fetching from mem) + 1 * memory latency (writing to mem)
- Cycles = 190
- Miss = 3
- Hit = 0

2) test if write and read misses that span two lines are detected correctly and result in correct amount of cycles 
````
W,0xF000001E,0xdddddddd
R,0x0F00001E,
R,0x00F0001E,
````
- Cycles = 3 * cache latency (accessing the cache) + 3 * memory latency (fetching each first line) + 3 * memory latency (fetching each second line) + 1 * memory latency (writing to memory)
- Cycles = 310
- Miss = 3
- Hit = 0

3) one write miss and three write hits with same tag to the same line to test if write hits work correctly and result in right amount of cycles
````
W,0xF0000020 (Miss)
W,0xF0000024 (Hit)
W,0xF0000028 (Hit)
W,0xF000002C (Hit)
````
- Cycles = 4 * cache latency (accessing the cache) + 1 * memory latency (fetching line in first write) + 4 * memory latency (writing values to memory) 
- Cycles = 240
- Miss = 1
- Hit = 3

4) four writes with different tags but in same line to test if write miss works correctly and result in right amount of cycles
````
W,0xF0000040,0xfafafafa
W,0xFF000041,0xfafafafa
W,0xFFF00042,0xfafafafa
W,0xFFFF0043,0xfafafafa
````
- Cycles = 4 * cache latency (accessing the cache) + 4 * mem latency (for each write) + 4 * mem latency (for each write to the memory)
- Cycles =  360
- Miss = 4
- Hit = 0

5) test write / read hits spanning two lines 
````
W,0xDDDDDDDE,0xfafafafa
W,0xDDDDDDDD,0xfafafafa
R,0xDDDDDDDF
````
- Cycles = 3 * cache latency (accessing the cache) + 2 * mem latency (fetching two lines in the cache) + 2 * mem latency (writing two values to memory)
- Cycles =  190
- Miss = 1
- Hit = 2

6) test wirte hits / read hits spanning two lines with one line already in cache 
````
W,0xAAAAAAA0 (Miss fetch one line)
W,0xAAAAAAAE (Miss fetch only one line becaues first line present in cache)
R,0xBBBBBBB0 (Miss push away 0xAAAAAAA0) 
R,0xAAAAAAAE (Miss but onyl fetch on line form mem)
````
- Cycles = 4 * cache latency + 2 * mem latency (first write - fetch on line and write to mem) + 2 * mem latency (fetch missing line and write to memory) + memory latency (fetch on line from memory) + memory latency (last read has to fetch only one line because on line is already present)
- Cycles = 280
- Miss = 4
- Hit = 0
