#include "../include/cpu.h"
#include <iostream>

CPU::CPU(Memory& mem) : memory(mem), pc(0), is_running(false), cycle_count(0), time_quantum(8), timer_interrupt_flag(false) { 
    reset(); 
}

void CPU::reset() {
    pc = 0;
    // Set SP to the top of our 20,000 byte memory
    sp = 19999; 
    registers.fill(0);
    is_running = true;
    cycle_count = 0;
    timer_interrupt_flag = false;
}

bool CPU::running() const {
    return is_running;
}

void CPU::step() {
    if (!is_running) return;
    
    // 1. FETCH the instruction (Opcode)
    uint8_t opcode = memory.read(pc);
    pc++; 
    
    // 2. DECODE and EXECUTE
    switch (opcode) {
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
        case OP_HALT: {
            is_running = false;
            std::cout << "CPU Halted.\n";
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
            
            if (registers[reg] != 0) {
                pc = jump_addr; 
            }
            break;
        }
        // --- NEW OPCODES FOR TASK 3 ---
        case OP_PUSH: {
            uint8_t reg = memory.read(pc++);
            sp--; // Stack grows downwards
            memory.write(sp, (uint8_t)registers[reg]); 
            break;
        }
        case OP_POP: {
            uint8_t reg = memory.read(pc++);
            registers[reg] = memory.read(sp);
            sp++; // Move pointer back up
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
    if (!is_running) return;
    step();
    cycle_count++;
    if (cycle_count >= time_quantum) {
        timer_interrupt_flag = true;
    }
}

void CPU::set_time_quantum(int cycles) { 
    time_quantum = cycles; 
}

bool CPU::has_interrupt() const { 
    return timer_interrupt_flag; 
}

void CPU::clear_interrupt() { 
    timer_interrupt_flag = false; 
    cycle_count = 0; 
}

void CPU::set_pc(size_t new_pc) {
    pc = new_pc;
}

size_t CPU::get_pc() const {
    return pc;
}

// --- NEW GETTERS/SETTERS FOR TASK 3 ---
void CPU::set_sp(size_t new_sp) {
    sp = new_sp;
}

size_t CPU::get_sp() const {
    return sp;
}

void CPU::set_registers(const uint32_t* regs) {
    for (int i = 0; i < 4; i++) {
        registers[i] = regs[i];
    }
}

void CPU::get_registers(uint32_t* regs) const {
    for (int i = 0; i < 4; i++) {
        regs[i] = registers[i];
    }
}

void CPU::run() {
    while (is_running) {
        step();
    }
}