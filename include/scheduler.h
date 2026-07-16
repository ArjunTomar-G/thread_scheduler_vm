#pragma once
#include <queue>
#include <vector>
#include <cstdint>
#include <memory>
#include <unordered_map>

// STATES :
enum class ThreadState {
    READY,
    RUNNING,
    BLOCKED,
    TERMINATED
};

// CPU Context
struct VM_Context {
    uint32_t instruction_pointer; 
    uint32_t stack_pointer;       
    uint32_t registers[4];        
};

// TCB
struct TCB {
    int thread_id;
    ThreadState state;
    VM_Context context;
    int total_execution_time; 
    
    // NEW: MLFQ Tracking
    int priority_level; // 0 = High, 1 = Medium, 2 = Low
};

// NEW: Mutex Resource Block
struct Mutex {
    bool is_locked = false;
    int owner_thread_id = -1;
    std::queue<TCB*> wait_queue; // Threads blocked waiting for this lock
};

class Scheduler {
private:
    static const int NUM_PRIORITIES = 3;
    std::vector<std::queue<TCB*>> mlfq;
    
    std::vector<std::unique_ptr<TCB>> all_threads;
    TCB* current_thread;
    int next_thread_id;

    std::unordered_map<int, Mutex> mutexes;
    int cycle_counter;

    // NEW: Bump Allocator for Thread Stacks
    uint32_t next_free_stack;

public:
    Scheduler();
    ~Scheduler() = default;
    
    int create_thread(uint32_t start_address);
    TCB* schedule_next();
    TCB* get_current_thread();

    // NEW: Terminate API
    void terminate_current_thread();

    bool lock_mutex(int mutex_id);
    void unlock_mutex(int mutex_id);
    void boost_priorities();
};