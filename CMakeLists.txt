cmake_minimum_required(VERSION 3.10)
project(dendrophone)

# Specify cross-compilation
set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR aarch64)
set(CMAKE_C_COMPILER aarch64-linux-gnu-gcc)
set(CMAKE_CXX_COMPILER aarch64-linux-gnu-g++)

set(CMAKE_CXX_STANDARD 17)

add_executable(program
    src/main.cpp
)

target_link_libraries(program
    gpiod
)
