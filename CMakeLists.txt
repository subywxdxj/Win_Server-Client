cmake_minimum_required(VERSION 3.10)

set (CMAKE_CXX_STANDARD 20)

project(Client-Server)

add_executable(Client Client/Client.cpp File.cpp File.h Includes.h)
add_executable(Server Server/Server.cpp File.cpp File.h Includes.h)