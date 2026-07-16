#include "../include/cpu.h"
#include <iostream>

CPU::CPU(Memory& mem) : memory(mem), pc(0), is_running(false), cycle_count(0), time_quantum(8), timer_interrupt_flag(false), soft_interrupt(SoftwareInterrupt::NONE), soft_interrupt_arg(0) { 
    reset(); 
}

void CPU::reset() {
    pc = 0;
    sp = 19999; 
    registers.fill(0);
    is_running = true;
    cycle_count = 0;
    timer_interrupt_flag = false;
    soft_interrupt = SoftwareInterrupt::NONE;
}

bool CPU::running() const { return is_running; }

void CPU::step() {
    if (!is_running) return;
    
    uint8_t opcode = memory.read(pc++);
    
    switch (opcode) {
        /* ... Keep OP_LOAD, OP_ADD, OP_PRINT, OP_SUB, OP_JNZ, OP_PUSH, OP_POP exactly as they are ... */
        case OP_LOAD: {
            uint8_t reg = memory.read(pc++);
            uint8_t val = memory.read(pc++);
            registers[reg] = val;
            break;
        }
        case OP_ADD: {
            uint8_t dest_reg = memory.read(pc++);
            uint8_t src_reg1 = memory.read(pc++);
            uint8_t src_reg2 = memory.read(pc++);
            registers[dest_reg] = registers[src_reg1] + registers[src_reg2];
            break;
        }
        case OP_PRINT: {
            uint8_t reg = memory.read(pc++);
            std::cout << "VM Output -> R" << (int)reg << " = " << registers[reg] << "\n";
            break;
        }
        case OP_SUB: {
            uint8_t dest_reg = memory.read(pc++);
            uint8_t src_reg1 = memory.read(pc++);
            uint8_t src_reg2 = memory.read(pc++);
            registers[dest_reg] = registers[src_reg1] - registers[src_reg2];
            break;
        }
        case OP_JNZ: {
            uint8_t reg = memory.read(pc++);
            uint8_t addr_high = memory.read(pc++);
            uint8_t addr_low = memory.read(pc++);
            uint16_t jump_addr = (addr_high << 8) | addr_low;
            if (registers[reg] != 0) pc = jump_addr; 
            break;
        }
        case OP_PUSH: {
            uint8_t reg = memory.read(pc++);
            sp--; 
            memory.write(sp, (uint8_t)registers[reg]); 
            break;
        }
        case OP_POP: {
            uint8_t reg = memory.read(pc++);
            registers[reg] = memory.read(sp);
            sp++; 
            break;
        }

        // --- NEW: Throw Software Interrupts instead of modifying state/OS directly ---
        case OP_HALT: {
            soft_interrupt = SoftwareInterrupt::HALT;
            break;
        }
        case OP_LOCK: {
            soft_interrupt_arg = memory.read(pc++);
            soft_interrupt = SoftwareInterrupt::LOCK;
            break;
        }
        case OP_UNLOCK: {
            soft_interrupt_arg = memory.read(pc++);
            soft_interrupt = SoftwareInterrupt::UNLOCK;
            break;
        }
        default: {
            std::cerr << "Unknown Opcode: 0x" << std::hex << (int)opcode << " at PC: " << (pc - 1) << "\n";
            is_running = false;
            break;
        }
    }
}

void CPU::tick() {
    // NEW: Stop ticking if a software interrupt is pending!
    if (!is_running || has_software_interrupt()) return;
    
    step();
    cycle_count++;
    if (cycle_count >= time_quantum) {
        timer_interrupt_flag = true;
    }
}

// --- NEW: Interrupt getters/setters ---
bool CPU::has_software_interrupt() const { return soft_interrupt != SoftwareInterrupt::NONE; }
SoftwareInterrupt CPU::get_software_interrupt() const { return soft_interrupt; }
uint8_t CPU::get_software_interrupt_arg() const { return soft_interrupt_arg; }

void CPU::clear_software_interrupt() { 
    soft_interrupt = SoftwareInterrupt::NONE; 
    soft_interrupt_arg = 0; 
}

/* ... Keep all other standard getters/setters (get_pc, set_pc, get_sp, etc.) exactly as they were ... */
void CPU::set_time_quantum(int cycles) { time_quantum = cycles; }
bool CPU::has_interrupt() const { return timer_interrupt_flag; }
void CPU::clear_interrupt() { timer_interrupt_flag = false; cycle_count = 0; }
void CPU::set_pc(size_t new_pc) { pc = new_pc; }
size_t CPU::get_pc() const { return pc; }
void CPU::set_sp(size_t new_sp) { sp = new_sp; }
size_t CPU::get_sp() const { return sp; }
void CPU::set_registers(const uint32_t* regs) { for (int i = 0; i < 4; i++) registers[i] = regs[i]; }
void CPU::get_registers(uint32_t* regs) const { for (int i = 0; i < 4; i++) regs[i] = registers[i]; }
void CPU::run() { while (is_running) { step(); } }