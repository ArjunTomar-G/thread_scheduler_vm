#include "../include/memory.h"
Memory::Memory(size_t size) {
    ram.resize(size, 0); // Fill RAM with 0s
}
uint8_t Memory::read(size_t address) const {
    if (address < ram.size()) {
        return ram[address];
    }
    std::cerr << "Memory Read Error: Out of bounds at address " << address << std::endl;
    return 0;
}

void Memory::write(size_t address, uint8_t value) {
    if (address < ram.size()) {
        ram[address] = value;
    } else {
        std::cerr << "Memory Write Error: Out of bounds at address " << address << std::endl;
    }
}
void Memory::load_program(const std::vector<uint8_t>& program, size_t start_address) {
    for (size_t i = 0; i < program.size(); ++i) {
        if (start_address + i < ram.size()) {
            ram[start_address + i] = program[i];
        }
    }
}
