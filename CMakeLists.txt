# cmake_minimum_required(VERSION <specify CMake version here>)
project(helloworld)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
add_executable(helloworld main.cpp)