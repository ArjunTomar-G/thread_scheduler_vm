#pragma once
#include "cpu.h"
#include "scheduler.h"

class Visualizer {
public:
    Visualizer();  
    ~Visualizer(); 
    void update(const CPU& cpu, Scheduler& scheduler);
};