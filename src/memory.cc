#include "memory.h"
#include <stdexcept>

Memory::Memory(uint32_t size) {
    if (size % 4 != 0) {
        throw std::invalid_argument("Memory allocation failed: Size must be a multiple of 4");
    }
    this->data = new char[size]; // Allocate memory
    this->size = size; // Set the size
}

Memory::~Memory() {
    delete[] data; // Deallocate memory
}

uint32_t Memory::read (uint32_t address) {
    if (address % 4 != 0) {
        printf("Error: Address must be a multiple of 4\n");
        return 0;
    }
    if (address >= size) {
        printf("Error: Address out of bounds\n");
        return 0;
    }
    return *(reinterpret_cast<uint32_t*>(data + address));
}

void Memory::write (uint32_t address, uint32_t value, uint8_t mask) {
    if (address % 4 != 0) {
        printf("Error: Address must be a multiple of 4\n");
        return;
    }
    if (address >= size) {
        printf("Error: Address out of bounds\n");
        return;
    }
    
    // selectively write bytes based on the mask
    while(mask) {
        if(mask & 0b1)
            *(reinterpret_cast<uint8_t*>(data + address)) = value & 0xff;
        
        mask >>= 1;
        value >>= 8;
        address++;
    }
}

void Memory::dump (uint32_t address, uint32_t size_w) {
    if (address % 4 != 0) {
        printf("Error: Address must be a multiple of 4\n");
        return;
    }
    if (address >= size) {
        printf("Error: Address out of bounds\n");
        return;
    }
    if (address + size_w * 4 > size) {
        printf("Error: Dump exceeds memory bounds\n");
        return;
    }
    for (uint32_t i = 0; i < size_w; i++) {
        printf("0x%08x\n", *(reinterpret_cast<uint32_t*>(data + address + (4*i))));
    }
    printf("\n");
}

void Memory::fill (uint32_t address, uint32_t size_w, uint32_t value) {
    if (address % 4 != 0) {
        printf("Error: Address must be a multiple of 4\n");
        return;
    }
    if (address >= size) {
        printf("Error: Address out of bounds\n");
        return;
    }
    if (address + size_w * 4 > size) {
        printf("Error: Fill exceeds memory bounds\n");
        return;
    }
    for (uint32_t i = 0; i < size_w; i++) {
        *(reinterpret_cast<uint32_t*>(data + address + (4*i))) = value;
    }
}

void Memory::load_hex(const std::string &filename) {
    // Load the hex file
    printf("Loading hex file: %s\n", filename.c_str());
    std::ifstream hex_file(filename);
    if(!hex_file.is_open()) {
        fprintf(stderr, "Error: Could not open hex file: %s\n", filename.c_str());
        return;
    }

    std::string line;
    uint32_t addr = 0x0000000;
    uint64_t nbytes_written = 0;

    while(std::getline(hex_file, line)) {
        if(line.empty()) continue;
        
        // Update address if line starts with '@'
        if(line[0] == '@') {
            addr = std::stoul(line.substr(1, line.size() - 1), nullptr, 16);
            continue;
        }

        // Parse the data line
        uint32_t data = std::stoul(line, nullptr, 16);

        // Write the data word to the memory
        write(addr, data);
        addr += 4;
        nbytes_written += 4;
    }

    hex_file.close();
    printf("Loaded %lu bytes in mem\n", nbytes_written);
}
