cmake_minimum_required(VERSION 3.7)
project(mini_reactor)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES main.cpp src/watcher.h src/loop.h)
add_executable(mini_reactor ${SOURCE_FILES})