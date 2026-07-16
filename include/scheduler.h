#pragma once
#include <queue>
#include <vector>
#include <cstdint>
#include <memory>

// STATES :
enum class ThreadState {
    READY,
    RUNNING,
    BLOCKED,
    TERMINATED
};

// CPU Context
struct VM_Context {
    uint32_t instruction_pointer; // Where in the program the thread paused
    uint32_t stack_pointer;       // Where the thread's stack is
    uint32_t registers[4];        // general-purpose registers (R0, R1, R2, R3)
};

// TCB
struct TCB {
    int thread_id;
    ThreadState state;
    VM_Context context;
    
    int total_execution_time; 
};

class Scheduler {
private:
    std::queue<TCB*> ready_queue;
    std::vector<std::unique_ptr<TCB>> all_threads;
    TCB* current_thread;
    int next_thread_id;

public:
    Scheduler();
    ~Scheduler() = default;
    
    // Creates a new thread and puts it in the ready queue
    int create_thread(uint32_t start_address);

    // Moves the current thread back to the queue and gets the next one
    TCB* schedule_next();

    // Returns a pointer to whoever is currently running
    TCB* get_current_thread();
};