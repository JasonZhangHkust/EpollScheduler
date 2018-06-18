cmake_minimum_required(VERSION 3.10)
project(Timer)


set(CMAKE_C_COMPILER /usr/local/bin/gcc CACHE PATH "")
set(CMAKE_CXX_COMPILER /usr/local/bin/g++ CACHE PATH "")
set(CMAKE_CXX_STANDARD 11)
SET(CMAKE_CXX_FLAGS            "${CMAKE_CXX_FLAGS} -g")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
add_executable(Timer Scheduler.h Scheduler.cpp Epoller.cpp Epoller.h main.cpp)