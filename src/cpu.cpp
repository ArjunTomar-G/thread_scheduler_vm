#include "../include/cpu.h"
#include <iostream>
// Opcodes for our tiny VM
const uint8_t OP_LOAD  = 0x01;
const uint8_t OP_ADD   = 0x02;
const uint8_t OP_PRINT = 0x03;
const uint8_t OP_HALT  = 0xFF;
CPU::CPU(Memory& mem) : memory(mem), pc(0), is_running(false) { reset(); }
void CPU::reset() {
    pc = 0;
    registers.fill(0);
    is_running = true;
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
            // LOAD format: [OP_LOAD] [Reg Index] [Value]
            uint8_t reg = memory.read(pc++);
            uint8_t val = memory.read(pc++);
            registers[reg] = val;
            break;
        }
        case OP_ADD: {
            // ADD format: [OP_ADD] [Dest Reg] [Src Reg 1] [Src Reg 2]
            uint8_t dest_reg = memory.read(pc++);
            uint8_t src_reg1 = memory.read(pc++);
            uint8_t src_reg2 = memory.read(pc++);
            registers[dest_reg] = registers[src_reg1] + registers[src_reg2];
            break;
        }
        case OP_PRINT: {
            // PRINT format: [OP_PRINT] [Reg Index]
            uint8_t reg = memory.read(pc++);
            std::cout << "VM Output -> R" << (int)reg << " = " << registers[reg] << "\n";
            break;
        }
        case OP_HALT: {
            // HALT format: [OP_HALT]
            is_running = false;
            std::cout << "CPU Halted.\n";
            break;
        }
        default: {
            std::cerr << "Unknown Opcode: 0x" << std::hex << (int)opcode << " at PC: " << (pc - 1) << "\n";
            is_running = false;
            break;
        }
    }
}

void CPU::set_pc(size_t new_pc) {
    pc = new_pc;
}

size_t CPU::get_pc() const {
    return pc;
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
