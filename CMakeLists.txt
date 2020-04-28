cmake_minimum_required(VERSION 3.16)
project(Anet)

set(CMAKE_CXX_STANDARD 11)
add_executable(Anet main.cpp ThreadPool.h Poller.h IOListener.h TCPConnection.h)
target_link_libraries(Anet pthread)
