cmake_minimum_required(VERSION 3.20)
project(VortexMem VERSION 1.0.0 LANGUAGES CXX)

# C++20 standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Compiler-specific optimization and warning flags
if(MSVC)
    add_compile_options(/W4 /WX /O2)
else()
    add_compile_options(-Wall -Wextra -Werror -O3 -pthread)
endif()

# Include directories
include_directories(include)

# Source files aggregation
file(GLOB_RECURSE SRC_FILES 
    "src/allocator/*.cpp"
    "src/cache/*.cpp"
    "src/paging/*.cpp"
    "src/server/*.cpp"
)

# Main Executable
add_executable(vortexmem src/main.cpp ${SRC_FILES})