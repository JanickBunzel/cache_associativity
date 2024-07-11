#include "memory.h"

// Default constructor
Memory::Memory(sc_module_name name, unsigned memoryLatency, unsigned cacheLineSize)
    : sc_module(name), memoryLatency(memoryLatency), cacheLineSize(cacheLineSize), readDataMEMORYOut(cacheLineSize)
{
    SC_THREAD(memoryAccess);
    sensitive << clkMEMORYIn.pos();
    printMemory();
}

// Write data to memory, crucial is that the data is written in the Little Endian order
void Memory::write(unsigned memoryAddress, sc_uint<32> data)
{
    for (unsigned i = 0; i < 4; ++i)
    {
        memory[memoryAddress + i] = (data >> (8 * i)) & 0xFF;
    }
}

std::vector<sc_uint<8>> Memory::readBlock(unsigned memoryAddress)
{
    unsigned blockStartAddr = memoryAddress - (memoryAddress % cacheLineSize);

    std::vector<sc_uint<8>> block(cacheLineSize);

    for (unsigned i = 0; i < cacheLineSize; ++i)
    {
        unsigned currentAddr = blockStartAddr + i;
        if (memory.find(currentAddr) == memory.end())
        {
            memory[currentAddr] = 0x00;
        }
        sc_uint<8> byte = memory[currentAddr];
        block[i] = byte;
    }
    return block;
}

void Memory::memoryAccess()
{
    while (true)
    {
        doneMEMORYOut.write(false);
        if (enableMEMORYIn.read() == true)
        {
            // TODO: Passt das mit dem memoryLatency oder muss nicht -1?
            for (unsigned i = 0; i < memoryLatency - 1; ++i)
            {
                // std::cout << "[Memory] Waiting memoryLatency" << std::endl;
                wait();
            }
            if (writeEnableMEMORYIn.read() == true)
            {
                write(addressMEMORYIn.read().to_uint(), writeDataMEMORYIn.read());
            }
            std::vector<sc_uint<8>> dataWord = readBlock(addressMEMORYIn.read().to_uint());
            for (unsigned i = 0; i < dataWord.size(); ++i)
            {
                readDataMEMORYOut[i].write(dataWord[i]);
            }
            doneMEMORYOut.write(true);
            // std::cout << "[Memory] Done" << std::endl;
            printMemory();
        }
        wait();
    }
}

void Memory::printMemory()
{
    std::cout << "Memory Content:" << std::endl;
    std::cout << "Address     Data (Hex)     Data (Binary)" << std::endl;
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

        std::cout << "0x" << std::setw(8) << std::setfill('0') << std::hex << address
                  << "   0x" << std::setw(2) << std::setfill('0') << std::hex << data.to_uint()
                  << "       " << std::bitset<8>(data.to_uint())
                  << std::dec << std::endl;
    }
}
