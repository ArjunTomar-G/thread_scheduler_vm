#include "scheduler.h"
#include "memory.h"
#include "cpu.h"
#include "visualizer.h"
#include <vector>
#include <unistd.h> 
#include <chrono>   
#include <fstream>  
#include <string>

// timer for the flame graph
long long get_time_us() {
    return std::chrono::duration_cast<std::chrono::microseconds>(
        std::chrono::high_resolution_clock::now().time_since_epoch()
    ).count();
}

int main() {
    Memory memory(10000);       
    CPU cpu(memory);     
    Scheduler scheduler; 

    std::vector<uint8_t> programA = { OP_LOAD, 0, 5, OP_LOAD, 1, 1, OP_PRINT, 0, OP_SUB, 0, 0, 1, OP_JNZ, 0, 0x10, 0x06, OP_HALT };
    std::vector<uint8_t> programB = { OP_LOAD, 0, 8, OP_LOAD, 1, 1, OP_PRINT, 0, OP_SUB, 0, 0, 1, OP_JNZ, 0, 0x20, 0x06, OP_HALT };
    memory.load_program(programA, 0x1000);
    memory.load_program(programB, 0x2000);

    scheduler.create_thread(0x1000);
    scheduler.create_thread(0x2000);

    TCB* current_thread = scheduler.schedule_next();
    Visualizer viz; // Boots up ncurses

    // Start JSON trace logger
    std::ofstream trace_file("vm_trace.json");
    trace_file << "[\n"; 
    bool first_event = true;

    auto log_event = [&](const std::string& name, long long start, long long end, int tid) {
        if (!first_event) trace_file << ",\n";
        trace_file << "  {\"name\": \"" << name << "\", \"cat\": \"Scheduler\", \"ph\": \"X\", \"ts\": " 
                   << start << ", \"dur\": " << (end - start) << ", \"pid\": 1, \"tid\": " << tid << "}";
        first_event = false;
    };

    const int TIME_QUANTUM = 3; 
    bool vm_running = true;

    while (vm_running) {
        if (current_thread == nullptr) break;

        // 1. Context Switch In
        long long switch_start = get_time_us();
        cpu.set_pc(current_thread->context.instruction_pointer);
        cpu.set_registers(current_thread->context.registers);
        long long switch_end = get_time_us();
        log_event("Context Switch (Load)", switch_start, switch_end, 0); 

        // 2. Thread Execution
        long long exec_start = get_time_us();
        
        for (int step = 0; step < TIME_QUANTUM; step++) {
            cpu.step();
            viz.update(cpu, scheduler); // Paint the screen
            usleep(250000); // 250ms pause so human eyes can track it

            if (!cpu.running()) { 
                vm_running = false; 
                break; 
            }
            current_thread->total_execution_time++;
        }
        
        long long exec_end = get_time_us();
        log_event("Execute Thread " + std::to_string(current_thread->thread_id), exec_start, exec_end, current_thread->thread_id);

        // 3. Context Switch Out
        long long save_start = get_time_us();
        current_thread->context.instruction_pointer = cpu.get_pc();
        cpu.get_registers(current_thread->context.registers);
        current_thread = scheduler.schedule_next();
        long long save_end = get_time_us();
        log_event("Context Switch (Save/Yield)", save_start, save_end, 0);

        if (current_thread != nullptr && current_thread->total_execution_time > 15) {
            vm_running = false; 
        }
    }

    trace_file << "\n]\n";
    trace_file.close();

    return 0; 
}