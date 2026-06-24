# Specify the minimum version for CMake
cmake_minimum_required(VERSION 3.10)

# Project Name and Language
project(ThreadSchedulerVM CXX)

# Set C++ standard to C++17 (good for modern features)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Tell CMake where to find our header files (.h)
include_directories(include)

# Gather all our source files (.cpp)
set(SOURCES
    src/main.cpp
    src/memory.cpp
    src/cpu.cpp
    src/scheduler.cpp
    src/visualizer.cpp
)

# Create the executable named "thread_scheduler_vm" using the source files
add_executable(thread_scheduler_vm ${SOURCES})

# Note for Phase 4: When you are ready to build the visualizer, 
# uncomment the lines below to link the ncurses library.
# find_package(Curses REQUIRED)
# include_directories(${CURSES_INCLUDE_DIR})
# target_link_libraries(thread_scheduler_vm ${CURSES_LIBRARIES})