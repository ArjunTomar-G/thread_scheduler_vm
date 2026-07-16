#include "scheduler.h"
#include "memory.h"
#include "cpu.h"
#include "visualizer.h"
#include <vector>
#include <chrono>   
#include <fstream>  
#include <string>

// High-res timer for the flame graph (Microseconds)
long long get_time_us() {
    return std::chrono::duration_cast<std::chrono::microseconds>(
        std::chrono::high_resolution_clock::now().time_since_epoch()
    ).count();
}

int main() {
    // 1. Initialize Hardware (Increased memory to hold 15 programs safely)
    Memory memory(20000);       
    CPU cpu(memory);     
    Scheduler scheduler; 
    cpu.attach_os(&scheduler);
    // 2 & 3. Dynamically Generate, Patch, and Load 15 Threads
    const int NUM_THREADS = 15;
    
    // Base program template: counts down from 30
    std::vector<uint8_t> template_prog = { 
        OP_LOAD, 0, 30,         // Load 30 into R0
        OP_LOAD, 1, 1,          // Load 1 into R1
        OP_PRINT, 0,            // Print R0 (Address offset + 0x06)
        OP_SUB, 0, 0, 1,        // R0 = R0 - R1
        OP_JNZ, 0, 0x00, 0x00,  // Jump to [To Be Patched]
        OP_HALT               
    };

    for (int i = 0; i < NUM_THREADS; i++) {
        // Space them 32 bytes (0x0020) apart in memory
        uint16_t base_addr = 0x1000 + (i * 0x0020); 
        
        // The instruction we want to jump back to (OP_PRINT) is 6 bytes into the program
        uint16_t jump_addr = base_addr + 0x06;
        
        // Patch the JNZ instruction with the correct high and low bytes
        template_prog[14] = (jump_addr >> 8) & 0xFF; // High byte
        template_prog[15] = jump_addr & 0xFF;        // Low byte

        // Load it into memory and spool up the thread
        memory.load_program(template_prog, base_addr);
        scheduler.create_thread(base_addr);
    }

    TCB* current_thread = scheduler.schedule_next();
    Visualizer viz; // Boots up ncurses

    // --- PROFILER SETUP ---
    std::ofstream trace_file("vm_trace.json");
    trace_file << "[\n"; 
    bool first_event = true;

    auto log_event = [&](const std::string& name, long long start, long long end, int tid) {
        if (!first_event) trace_file << ",\n";
        trace_file << "  {\"name\": \"" << name << "\", \"cat\": \"Scheduler\", \"ph\": \"X\", \"ts\": " 
                   << start << ", \"dur\": " << (end - start) << ", \"pid\": 1, \"tid\": " << tid << "}";
        first_event = false;
    };

    // Execution configuration
    cpu.set_time_quantum(8); 
    bool vm_running = true;

    while (vm_running) {
        if (current_thread == nullptr) break;

        // PROFILE: Context Switch (Load)
        long long switch_start = get_time_us();
        
        cpu.set_pc(current_thread->context.instruction_pointer);
        
        // --- NEW: Load the Stack Pointer safely ---
        if (current_thread->context.stack_pointer == 0) {
            // Allocate a unique 1KB stack frame based on thread_id
            cpu.set_sp(19999 - (current_thread->thread_id * 1000));
        } else {
            cpu.set_sp(current_thread->context.stack_pointer);
        }

        cpu.set_registers(current_thread->context.registers);
        long long switch_end = get_time_us();
        log_event("Context Switch (Load)", switch_start, switch_end, 0); 

        // PROFILE: Thread Execution
        long long exec_start = get_time_us();
        
        // The CPU runs until IT throws a hardware interrupt!
        while (!cpu.has_interrupt() && cpu.running()) {
            cpu.tick(); // Advance the CPU clock
            viz.update(cpu, scheduler); 
            
            current_thread->total_execution_time++;
        }
        
        // OS explicitly acknowledges and clears the hardware interrupt
        if (cpu.has_interrupt()) {
            cpu.clear_interrupt();
        }

        if (!cpu.running()) { 
            vm_running = false; 
            break; 
        }
        
        long long exec_end = get_time_us();
        log_event("Execute Thread " + std::to_string(current_thread->thread_id), exec_start, exec_end, current_thread->thread_id);

        // PROFILE: Context Switch (Save & Yield)
        long long save_start = get_time_us();
        
        current_thread->context.instruction_pointer = cpu.get_pc();
        // --- NEW: Save the Stack Pointer back to the TCB ---
        current_thread->context.stack_pointer = cpu.get_sp();
        cpu.get_registers(current_thread->context.registers);
        
        current_thread = scheduler.schedule_next();
        long long save_end = get_time_us();
        log_event("Context Switch (Save/Yield)", save_start, save_end, 0);

        // Safety limit extended to 500 for the 15-thread horde
        if (current_thread != nullptr && current_thread->total_execution_time > 500) {
            vm_running = false; 
        }
    }

    trace_file << "\n]\n";
    trace_file.close();

    return 0; 
}