cmake_minimum_required(VERSION 2.8)
project(SimpleCalc)

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
set (SOURCE_EXE SimpleCalc.cpp)
add_executable(${PROJECT_NAME} ${SOURCE_EXE})
