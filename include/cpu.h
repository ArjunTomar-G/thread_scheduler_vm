#ifndef CPU_H
#define CPU_H
#include "memory.h"
#include <array>
#include <cstdint>
class CPU {
private:
    Memory& memory;
    std::array<int, 4> registers; // 4 general-purpose registers: R0, R1, R2, R3
    size_t pc;                    // Program Counter (tracks current memory address)
    bool is_running;              // Is the CPU currently executing?
public:
    CPU(Memory& mem);
    void reset();         // Reset PC and registers
    void step();          // Fetch, Decode, and Execute ONE instruction
    void run();           // Run continuously until HALT
    bool running() const; // Check if CPU is still running
    void set_pc(size_t new_pc);
    size_t get_pc() const;
    void set_registers(const uint32_t* regs);
    void get_registers(uint32_t* regs) const;
};
#endif // CPU_H
