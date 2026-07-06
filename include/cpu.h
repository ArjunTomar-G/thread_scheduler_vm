#ifndef CPU_H
#define CPU_H
#include "memory.h"
#include <array>
#include <cstdint>
constexpr uint8_t OP_LOAD  = 0x01;
constexpr uint8_t OP_ADD   = 0x02;
constexpr uint8_t OP_PRINT = 0x03;
constexpr uint8_t OP_SUB   = 0x04;
constexpr uint8_t OP_JNZ   = 0x05;
constexpr uint8_t OP_HALT  = 0xFF;
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
