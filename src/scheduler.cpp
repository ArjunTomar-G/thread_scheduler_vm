#include "../include/scheduler.h"
#include <iostream>

Scheduler::Scheduler() {
    current_thread = nullptr;
    next_thread_id = 1;
    cycle_counter = 0;
    mlfq.resize(NUM_PRIORITIES); 
}

int Scheduler::create_thread(uint32_t start_address) {
    auto new_thread = std::make_unique<TCB>();
    
    new_thread->thread_id = next_thread_id++;
    new_thread->state = ThreadState::READY;
    new_thread->total_execution_time = 0;
    new_thread->priority_level = 0; // Start at highest priority
    
    new_thread->context.instruction_pointer = start_address;
    new_thread->context.stack_pointer = 0; 
    for (int i = 0; i < 4; i++) {
        new_thread->context.registers[i] = 0;
    }

    TCB* thread_ptr = new_thread.get();
    all_threads.push_back(std::move(new_thread));
    
    mlfq[0].push(thread_ptr); // Push to highest priority queue
    
    std::cout << "[Scheduler] Created Thread ID " << thread_ptr->thread_id << " starting at address " << start_address << "\n";
    return thread_ptr->thread_id;
}

TCB* Scheduler::schedule_next() {
    cycle_counter++;
    
    // Starvation prevention: Every 50 switches, bump everyone back to Priority 0
    if (cycle_counter % 50 == 0) {
        boost_priorities();
    }

    if (current_thread != nullptr) {
        if (current_thread->state == ThreadState::RUNNING) {
            // Demote CPU-heavy threads
            if (current_thread->priority_level < NUM_PRIORITIES - 1) {
                current_thread->priority_level++; 
            }
            current_thread->state = ThreadState::READY;
            mlfq[current_thread->priority_level].push(current_thread);
            std::cout << "[Scheduler] Thread " << current_thread->thread_id << " demoted to Queue " << current_thread->priority_level << "\n";
        }
        // Note: If state is BLOCKED, it does NOT go back into the mlfq here.
    }

    // Find the highest priority queue with available threads
    current_thread = nullptr;
    for (int i = 0; i < NUM_PRIORITIES; i++) {
        if (!mlfq[i].empty()) {
            current_thread = mlfq[i].front();
            mlfq[i].pop();
            break;
        }
    }

    if (current_thread != nullptr) {
        current_thread->state = ThreadState::RUNNING;
        std::cout << "[Scheduler] Context Switch -> Now running Thread ID " << current_thread->thread_id << " (Queue " << current_thread->priority_level << ")\n";
    }

    return current_thread;
}

bool Scheduler::lock_mutex(int mutex_id) {
    if (current_thread == nullptr) return false;

    Mutex& m = mutexes[mutex_id];

    if (!m.is_locked) {
        m.is_locked = true;
        m.owner_thread_id = current_thread->thread_id;
        std::cout << "[Mutex] Thread " << current_thread->thread_id << " acquired Lock " << mutex_id << "\n";
        return true;
    } 
    
    // Lock taken. Block the thread.
    current_thread->state = ThreadState::BLOCKED;
    m.wait_queue.push(current_thread);
    std::cout << "[Mutex] Thread " << current_thread->thread_id << " BLOCKED on Lock " << mutex_id << "\n";
    return false; // Tells the CPU to halt immediately!
}

void Scheduler::unlock_mutex(int mutex_id) {
    if (current_thread == nullptr) return;

    Mutex& m = mutexes[mutex_id];

    if (m.owner_thread_id == current_thread->thread_id) {
        std::cout << "[Mutex] Thread " << current_thread->thread_id << " released Lock " << mutex_id << "\n";
        
        if (!m.wait_queue.empty()) {
            TCB* next_thread = m.wait_queue.front();
            m.wait_queue.pop();
            
            next_thread->state = ThreadState::READY;
            mlfq[next_thread->priority_level].push(next_thread); // Put back into OS queue
            m.owner_thread_id = next_thread->thread_id; // Pass ownership
            
            std::cout << "[Mutex] Thread " << next_thread->thread_id << " unblocked and given Lock " << mutex_id << "\n";
        } else {
            m.is_locked = false;
            m.owner_thread_id = -1;
        }
    }
}

void Scheduler::boost_priorities() {
    std::cout << "[Scheduler] Boosting all priorities to prevent starvation.\n";
    for (int i = 1; i < NUM_PRIORITIES; i++) {
        while (!mlfq[i].empty()) {
            TCB* thread = mlfq[i].front();
            mlfq[i].pop();
            thread->priority_level = 0;
            mlfq[0].push(thread);
        }
    }
}

TCB* Scheduler::get_current_thread() {
    return current_thread;
}