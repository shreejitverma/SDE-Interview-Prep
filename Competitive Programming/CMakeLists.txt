cmake_minimum_required(VERSION 3.20)
project(Competitive_Programming)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)
include_directories(Sorting)

add_executable(Competitive_Programming
        "Sorting/bubble sort.cpp"
        "Sorting/selection sort.cpp")
