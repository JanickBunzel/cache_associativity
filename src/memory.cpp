#include <bitset>
#include <iomanip>
#include "memory.h"

// Global variables provided by the rahmenprogramm (cache_simulaton option), specifies how the debug information is printed
extern int printsLevel;
extern char *highlightDataColor;
extern char *resetColor;

// Default constructor
Memory::Memory(sc_module_name name, unsigned memoryLatency, unsigned cachelineSize)
    : sc_module(name), memoryLatency(memoryLatency), cachelineSize(cachelineSize), readDataMEMORYOut(cachelineSize)
{
    SC_THREAD(memoryAccess);
    sensitive << clkMEMORYIn.pos();
    dont_initialize();

    printMemory();
}

// Main method for all memory usage
void Memory::memoryAccess()
{
    while (true)
    {
        doneMEMORYOut.write(false);
        if (enableMEMORYIn.read() == true)
        {
            // Await the memory latency
            for (unsigned i = 0; i < memoryLatency - 1; ++i)
            {
                wait();
            }

            // If the request is a write request, write the data to the memory
            if (writeEnableMEMORYIn.read() == true)
            {
                write(addressMEMORYIn.read().to_uint(), writeDataMEMORYIn.read());
            }

            else
            {
                std::vector<sc_uint<8>> dataWord = readBlock(addressMEMORYIn.read().to_uint());
                // write to the output ports
                for (unsigned i = 0; i < dataWord.size(); ++i)
                {
                    if (i < readDataMEMORYOut.size()) // Ensure we do not go out of bounds
                    {
                        readDataMEMORYOut[i].write(dataWord[i]);
                    }
                }
            }

            doneMEMORYOut.write(true);
            printMemory();
        }
        wait();
    }
}

// Write data to the address, if it is not present in the memory, it is added first and then written to
void Memory::write(unsigned memoryAddress, sc_uint<32> data)
{
    // write byte by byte with LSB at the highest address
    for (unsigned i = 0; i < 4; ++i)
    {
        memory[memoryAddress + (3 - i)] = (data >> (8 * i)) & 0xFF;
    }
}

std::vector<sc_uint<8>> Memory::readBlock(unsigned memoryAddress)
{
    // Calculate the start address of the block
    unsigned blockStartAddr = memoryAddress - (memoryAddress % cachelineSize);
    // block is the cache line that is returned
    std::vector<sc_uint<8>> block(cachelineSize);
    // fetch all bytes from the memory map starting at the given block address
    for (unsigned i = 0; i < cachelineSize; ++i)
    {
        // If the address is not present in the memory, add it with the value 0x00
        if (memory.find(blockStartAddr + i) == memory.end())
        {
            memory[blockStartAddr + i] = 0x00;
        }
        // Read the byte from the memory map
        sc_uint<8> byte = memory[blockStartAddr + i];
        // Write the byte to the block
        block[i] = byte;
    }
    return block;
}

void Memory::printMemory()
{
    if (printsLevel < 2)
    {
        return;
    }

    std::cout << "[Memory]: Memory Content:" << std::endl;
    std::cout << "-----------------------------------------" << std::endl;
    std::cout << "Address      Data(Hex)  Data(Binary)" << std::endl;
    std::cout << "-----------------------------------------" << std::endl;

    // Copy memory entries to a vector and sort them by address
    std::vector<std::pair<unsigned, sc_uint<8>>> sorted_memory(memory.begin(), memory.end());
    std::sort(sorted_memory.begin(), sorted_memory.end(),
              [](const std::pair<unsigned, sc_uint<8>> &a, const std::pair<unsigned, sc_uint<8>> &b)
              {
                  return a.first < b.first;
              });

    // Print sorted memory
    for (const auto &entry : sorted_memory)
    {
        unsigned address = entry.first;
        sc_uint<8> data = entry.second;

        if (data.to_uint() != 0 && printsLevel > 0)
        {
            std::cout << highlightDataColor; // Highlight present data
        }

        // Address
        std::cout << "0x" << std::setw(8) << std::setfill('0') << std::hex << address;
        
        // Data
        std::cout << "   0x" << std::setw(2) << std::setfill('0') << std::hex << data.to_uint()
                  << "       " << std::bitset<8>(data.to_uint())
                  << std::dec << std::endl;
        if (data.to_uint() != 0 && printsLevel > 0)
        {
            std::cout << resetColor; // Reset the color
        }
    }
}
