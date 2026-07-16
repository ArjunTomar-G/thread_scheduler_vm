#include "visualizer.h"
#include <ncurses.h>

Visualizer::Visualizer() {
    initscr();             
    cbreak();              
    noecho();              
    curs_set(0);           
    clear();               
}

Visualizer::~Visualizer() {
    endwin();              
}

void Visualizer::update(const CPU& cpu, Scheduler& scheduler) {
    clear(); 

    box(stdscr, 0, 0);
    mvprintw(0, 2, " [ Thread Scheduler VM Dashboard ] ");

    // CPU State
    mvprintw(2, 2, "--- CPU STATE ---");
    mvprintw(3, 2, "Program Counter (PC): 0x%04zX", cpu.get_pc());
    
    uint32_t regs[4];
    cpu.get_registers(regs);
    for(int i = 0; i < 4; i++) {
        mvprintw(4 + i, 2, "R%d: %d", i, regs[i]);
    }

    // Scheduler State
    mvprintw(9, 2, "--- SCHEDULER STATE ---");
    TCB* current = scheduler.get_current_thread();
    if (current != nullptr) {
        mvprintw(10, 2, "Active Thread ID : %d", current->thread_id);
        mvprintw(11, 2, "Execution Time   : %d cycles", current->total_execution_time);
    } else {
        mvprintw(10, 2, "Active Thread ID : NONE (IDLE)");
    }

    refresh();
}