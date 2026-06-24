#include "scheduler.h"
#include <iostream>

Scheduler::Scheduler() {
    current_thread = nullptr;
    next_thread_id = 1;
}

Scheduler::~Scheduler() {
    // Clean up memory when the VM shuts down
    for (TCB* thread : all_threads) {
        delete thread;
    }
}

int Scheduler::create_thread(uint32_t start_address) {
    // Allocate a new TCB on the heap
    TCB* new_thread = new TCB();
    
    // Initialize 
    new_thread->thread_id = next_thread_id++;
    new_thread->state = ThreadState::READY;
    new_thread->total_execution_time = 0;
    
    // Context
    new_thread->context.instruction_pointer = start_address;
    new_thread->context.stack_pointer = 0; // Teammate A's memory will handle real stack addresses later
    for (int i = 0; i < 4; i++) {
        new_thread->context.registers[i] = 0;
    }
    all_threads.push_back(new_thread);
    ready_queue.push(new_thread);
    
    std::cout << "[Scheduler] Created Thread ID " << new_thread->thread_id << " starting at address " << start_address << "\n";
    
    return new_thread->thread_id;
}

TCB* Scheduler::schedule_next() {
    // If there is no one waiting, just keep running the current thread
    if (ready_queue.empty()) {
        return current_thread;
    }

    // If a thread is currently running, we need to pause it and put it back in line
    if (current_thread != nullptr && current_thread->state == ThreadState::RUNNING) {
        current_thread->state = ThreadState::READY;
        ready_queue.push(current_thread);
        std::cout << "[Scheduler] Thread ID " << current_thread->thread_id << " yielded. Moving to Ready Queue.\n";
    }

    // Pop the next thread off the front of the queue
    current_thread = ready_queue.front();
    ready_queue.pop();
    
    // Mark it as running
    current_thread->state = ThreadState::RUNNING;
    std::cout << "[Scheduler] Context Switch -> Now running Thread ID " << current_thread->thread_id << "\n";

    // We return the TCB so the CPU (Teammate A's code) can load its virtual registers!
    return current_thread;
}

TCB* Scheduler::get_current_thread() {
    return current_thread;
}
