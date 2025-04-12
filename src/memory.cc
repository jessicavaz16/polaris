#include "memory.h"
#include <stdexcept>

Memory::Memory(int size) {
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

void Memory::write (uint32_t address, uint32_t value) {
    if (address % 4 != 0) {
        printf("Error: Address must be a multiple of 4\n");
        return;
    }
    if (address >= size) {
        printf("Error: Address out of bounds\n");
        return;
    }
    *(reinterpret_cast<uint32_t*>(data + address)) = value;
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
