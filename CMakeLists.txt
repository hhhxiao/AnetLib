cmake_minimum_required(VERSION 3.16)
project(Anet)

set(CMAKE_CXX_STANDARD 11)
add_executable(Anet main.cpp ThreadPool.h Poller.h IOListener.h TCPServer.h utils.h utils.cpp TcpConnection.h DataBuffer.h)
add_executable(UnixTest UnixTest.c)
target_link_libraries(Anet pthread)
