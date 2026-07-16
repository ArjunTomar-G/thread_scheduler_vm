#pragma once
#include "memory.h"
#include <array>
#include <cstdint>
class Scheduler;
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

class CPU {
private:
    Memory& memory;
    std::array<int, 4> registers; // 4 general-purpose registers: R0, R1, R2, R3
    size_t pc;                    // Program Counter (tracks current memory address)
    size_t sp;
    Scheduler* os = nullptr;
    bool is_running;              // Is the CPU currently executing?

    // Hardware Timer & Interrupt State 
    int cycle_count;
    int time_quantum;
    bool timer_interrupt_flag;

public:
    CPU(Memory& mem);
    void reset();         // Reset PC and registers
    void step();          // Fetch, Decode, and Execute ONE instruction

    // Hardware Interrupt Methods 
    void tick();                       // Advances the CPU clock by 1 cycle
    void set_time_quantum(int cycles); // Allows the OS to set the timer
    bool has_interrupt() const;        // OS checks this to see if preemption is needed
    void clear_interrupt();            // OS resets the timer for the next thread

    void run();           // Run continuously until HALT
    bool running() const; // Check if CPU is still running
    void set_pc(size_t new_pc);
    size_t get_pc() const;

    void set_sp(size_t new_sp);
    size_t get_sp() const;
    
    void set_registers(const uint32_t* regs);
    void get_registers(uint32_t* regs) const;
    void attach_os(Scheduler* scheduler_ptr) { os = scheduler_ptr; }
};

