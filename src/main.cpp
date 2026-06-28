#include "scheduler.h"
#include <iostream>

int main() {
    std::cout << "Starting VM Scheduler Test...\n";
    Scheduler sched;
    
    sched.create_thread(0x1000); // Fake thread 1
    sched.create_thread(0x2000); // Fake thread 2
    
    // Simulate context switching
    sched.schedule_next();
    sched.schedule_next();
    sched.schedule_next();
    
    return 0;
}