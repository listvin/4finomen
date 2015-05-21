cmake_minimum_required(VERSION 3.2)
project(HW2_ConwaysLife)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES main.cpp)
add_executable(HW2_ConwaysLife ${SOURCE_FILES} world.hpp)
