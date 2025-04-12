#include "memory.h"
#include <stdexcept>
#include <iostream>

int main() {
    try {
        Memory mem(1022); // Create a memory object with 1024 bytes
        mem.fill(0, 4, 0xFFFFFFFE); // Fill the first 4 words with 0xFF
        mem.dump(0, 4); // Dump the first 4 words of memory

        uint32_t read_value = mem.read(0x00000002); // Read from address 0x00000000
        printf("Read value: 0x%08x\n", read_value); // Print the read value

        mem.write(0x00000002, 0x12345678); // Write 0x12345678 to address 0x00000004
        read_value = mem.read(0x00000000);
        printf("Read value: 0x%08x\n", read_value);
    }
    catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }
}   
