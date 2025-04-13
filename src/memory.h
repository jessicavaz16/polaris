#pragma once
#include <stdint.h>
#include <stdio.h>
#include <fstream>

class Memory {
    private:
        char *data; // Pointer to the memory block
        uint32_t size;   // Size of the memory block in bytes

    public:
        // Constructor 
        Memory(uint32_t size = 1024);

        // Destructor
        ~Memory();

        // Read data from the specified address
        uint32_t read (uint32_t address);

        // Write data to the specified address
        void write (uint32_t address, uint32_t value, uint8_t mask = 0b1111);

        // Dump memory contents from a specified address
        void dump (uint32_t address, uint32_t size_w);

        // Fill memory with a specified value
        void fill (uint32_t address, uint32_t size_w, uint32_t value);

        // Load memory contents from a hex file
        void load_hex(const std::string &filename);
};
