#include "scheduler.h"
#include "memory.h"
#include "cpu.h"
#include <iostream>

int main() {
    std::cout << "Starting Thread Scheduler VM \n";

    // 1. Initialize Hardware and OS
    Memory memory(10000);       
    CPU cpu(memory);     
    Scheduler scheduler; 

    // 2. Define the Programs (Teammate B's code goes here)
    // Program A: Counts down from 5
    std::vector<uint8_t> programA = {
        OP_LOAD, 0, 5,         // 0x1000: Load 5 into R0
        OP_LOAD, 1, 1,         // 0x1003: Load 1 into R1
        OP_PRINT, 0,           // 0x1006: Print R0
        OP_SUB, 0, 0, 1,       // 0x1008: R0 = R0 - R1
        OP_JNZ, 0, 0x10, 0x06, // 0x100C: If R0 != 0, jump to 0x1006
        OP_HALT                // 0x1010: Halt
    };

    // Program B: Counts down from 8 (Starts at address 0x2000)
    std::vector<uint8_t> programB = {
        OP_LOAD, 0, 8,         // 0x2000: Load 8 into R0
        OP_LOAD, 1, 1,         // 0x2003: Load 1 into R1
        OP_PRINT, 0,           // 0x2006: Print R0
        OP_SUB, 0, 0, 1,       // 0x2008: R0 = R0 - R1
        OP_JNZ, 0, 0x20, 0x06, // 0x200C: If R0 != 0, jump to 0x2006
        OP_HALT                // 0x2010: Halt
    };

    // loading programs in memory:
    memory.load_program(programA, 0x1000);
    memory.load_program(programB, 0x2000);

    // thread creation:
    scheduler.create_thread(0x1000);
    scheduler.create_thread(0x2000);


    // 3. Start the first thread
    TCB* current_thread = scheduler.schedule_next();

    // 4. VM Loop
    const int TIME_QUANTUM = 5; 
    bool vm_running = true;

    while (vm_running) {
        if (current_thread == nullptr) {
            std::cout << "No threads to run. Shutting down VM.\n";
            break;
        }

        // CONTEXT SWITCH: LOAD 
        // Give the CPU the saved registers and PC from the Thread Control Block
        cpu.set_pc(current_thread->context.instruction_pointer);
        cpu.set_registers(current_thread->context.registers);
        std::cout << "\n[VM Clock] Loaded Thread " << current_thread->thread_id << " to CPU.\n";

        // EXECUTION
        // Let the CPU run for a specific number of cycles (Time Quantum)
        for (int step = 0; step < TIME_QUANTUM; step++) {
            
            cpu.step();
            
            if (!cpu.running()) { 
                std::cout << "  -> Thread " << current_thread->thread_id << " finished execution (Halted).\n";
                vm_running = false; 
                break; 
            }
            
            std::cout << "  -> Thread " << current_thread->thread_id << " executing instruction...\n";
            current_thread->total_execution_time++;
        }

        // CONTEXT SWITCH: SAVE 
        // Take the current registers BACK from the CPU
        current_thread->context.instruction_pointer = cpu.get_pc();
        cpu.get_registers(current_thread->context.registers);
        std::cout << "[VM Clock] Time quantum expired. Saving Thread " << current_thread->thread_id << " context.\n";

        // SCHEDULER: YIELD AND GET NEXT 
        // Put the current thread in the back of the queue and grab the next one
        current_thread = scheduler.schedule_next();

 
        if (current_thread->total_execution_time > 15) {
            std::cout << "\n[Test Limit Reached] Stopping VM to prevent infinite loop.\n";
            vm_running = false; 
        }
    }

    std::cout << "VM Shutdown Complete \n";
    return 0;
}