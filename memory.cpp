#include "memory.h"

void Memory::write(sc_uint<> addr, sc_uint<32> data)
{
    for (int i = 0; i < 4; ++i)
    {
        memory[addr + i] = (data >> (i * 8)) & 0xFF;
    }
}

sc_uint<32> Memory::read(unsigned addr)
{
    sc_uint<32> data = 0;
    for (int i = 0; i < 4; ++i)
    {
        if (memory.find(addr + i) == memory.end())
        {
            sc_uint<8> arbitrary_data = rand() & 0xFF;
            memory[addr + i] = arbitrary_data;
            data |= arbitrary_data << (i * 8);
        }
        else
        {
            data |= memory[addr + i] << (i * 8);
        }
    }
    return data;
}

void Memory::memoryAccess()
{
    while (true)
    {
        wait(); // Wait for the clock

        if (we.read() == true) // Write operation
        {
            write(address.read().to_uint(), wdata.read());
            wait(memoryLatency); // Simulate memory latency
            memoryDone.write(true);
        }
        else // Read operation
        {
            rdata.write(read(address.read().to_uint()));
            wait(memoryLatency); // Simulate memory latency
            memoryDone.write(true);
        }
    }
}