cmake_minimum_required(VERSION 3.10)
project(Timer)

set(CMAKE_CXX_STANDARD 11)

add_executable(Timer main.cpp Timer.h Timer.cpp Epoller.cpp Epoller.h)