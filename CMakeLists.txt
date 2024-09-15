cmake_minimum_required(VERSION 3.16)

project(http-server)

add_library(tcpserver tcpserver.c)

add_library(httpserver httpserver.c)
target_link_libraries(httpserver PUBLIC tcpserver)

add_executable(test test.c)
target_link_libraries(test PUBLIC httpserver)