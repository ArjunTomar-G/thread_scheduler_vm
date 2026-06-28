#include "scheduler.h"
#include <iostream>
#include "../include/memory.h"
#include "../include/cpu.h"
#include <vector>

int main() {
    std::cout << "Starting VM Scheduler Test...\n";
    Scheduler sched;
    std::cout << "--- Starting Phase 1 VM Test ---\n";
    // 1. Initialize our simulated hardware
    Memory ram;
    CPU cpu(ram);
    // 2. Write our hardcoded program (5 + 5)
    // We are hand-compiling our custom Assembly into machine code (bytes)
    std::vector<uint8_t> program = {
        0x01, 0x00, 0x05,  // LOAD R0, 5    (Opcode 01, Reg 0, Val 5)
        0x01, 0x01, 0x05,  // LOAD R1, 5    (Opcode 01, Reg 1, Val 5)
        0x02, 0x02, 0x00, 0x01, // ADD R2, R0, R1 (Opcode 02, Dest R2, Src R0, Src R1)
        0x03, 0x02,        // PRINT R2      (Opcode 03, Reg 2) - Should print 10!
        0xFF               // HALT          (Opcode FF)
    };
    // 3. Load the program into RAM starting at address 0
    ram.load_program(program, 0);
    // 4. Run the CPU
    cpu.run();
    std::cout << "--- Test Complete ---\n";
    return 0;
}

