cmake_minimum_required(VERSION 3.16)
project(Anet)

set(CMAKE_CXX_STANDARD 11)
add_executable(Anet main.cpp ThreadPool.h Poller.h IOListener.h TCPServer.h utils.h utils.cpp TcpConnection.h DataBuffer.h)
add_executable(Test SimpleTest.cpp utils.cpp)
add_executable(Client Client/client.cpp utils.cpp)
add_executable(FileGenerator Client/fileGen.c)
target_link_libraries(Anet pthread)
target_link_libraries(Client pthread)
