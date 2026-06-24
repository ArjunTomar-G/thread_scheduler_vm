#ifndef MEMORY_H
#define MEMORY_H
#include <vector>
#include <cstdint>
#include <iostream>
class Memory {
private:
    std::vector<uint8_t> ram;
public:
    // Initialize memory with a default size (e.g., 1024 bytes)
    Memory(size_t size = 1024);
    // Read a byte from a specific address
    uint8_t read(size_t address) const;
    // Write a byte to a specific address
    void write(size_t address, uint8_t value);
    // Load a hardcoded byte array (our program) into memory
    void load_program(const std::vector<uint8_t>& program, size_t start_address = 0);
};
#endif // MEMORY_H
