cmake_minimum_required(VERSION 3.2)
project(Avian3)

include_directories(Sources)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

add_executable(Avian3 main.cc Sources/jit/Assembler.h Sources/jit/Assembler.cc Sources/jit/Assembler_x64.h Sources/jit/Constants_x64.h Sources/jit/Assembler_x64.cc)