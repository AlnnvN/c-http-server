cmake_minimum_required(VERSION 3.16)

project(http-server)

add_library(tcpserver tcpserver.c)

add_executable(server server.c)
target_link_libraries(server PUBLIC tcpserver)