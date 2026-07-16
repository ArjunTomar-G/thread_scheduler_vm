#pragma once
#include "memory.h"
#include <array>
#include <cstdint>

constexpr uint8_t OP_LOAD  = 0x01;
constexpr uint8_t OP_ADD   = 0x02;
constexpr uint8_t OP_PRINT = 0x03;
constexpr uint8_t OP_SUB   = 0x04;
constexpr uint8_t OP_JNZ   = 0x05;
constexpr uint8_t OP_PUSH  = 0x06; 
constexpr uint8_t OP_POP   = 0x07; 
constexpr uint8_t OP_HALT  = 0xFF;
constexpr uint8_t OP_LOCK   = 0x08; 
constexpr uint8_t OP_UNLOCK = 0x09;

// NEW: Define Software Interrupts (Syscalls)
enum class SoftwareInterrupt {
    NONE,
    HALT,
    LOCK,
    UNLOCK
};

class CPU {
private:
    Memory& memory;
    std::array<int, 4> registers;
    size_t pc;
    size_t sp;
    bool is_running;

    int cycle_count;
    int time_quantum;
    bool timer_interrupt_flag;

    // NEW: Software Interrupt State
    SoftwareInterrupt soft_interrupt;
    uint8_t soft_interrupt_arg; 

public:
    CPU(Memory& mem);
    void reset();
    void step();

    void tick();
    void set_time_quantum(int cycles);
    
    bool has_interrupt() const;
    void clear_interrupt();

    // NEW: Software Interrupt API
    bool has_software_interrupt() const;
    SoftwareInterrupt get_software_interrupt() const;
    uint8_t get_software_interrupt_arg() const;
    void clear_software_interrupt();

    void run();
    bool running() const;
    void set_pc(size_t new_pc);
    size_t get_pc() const;

    void set_sp(size_t new_sp);
    size_t get_sp() const;
    
    void set_registers(const uint32_t* regs);
    void get_registers(uint32_t* regs) const;
};