cmake_minimum_required(VERSION 3.10)

set (CMAKE_CXX_STANDARD 20)
ADD_DEFINITIONS(-DUNICODE)

project(Client-Server)

file(GLOB ROOT CONFIGURE_DEPENDS *.cpp *.hpp)

add_executable(Client Client/Client.cpp Client/CommandClient.cpp Client/CommandClient.hpp ${ROOT})
add_executable(Server Server/Server.cpp Server/CommandServer.cpp Server/CommandServer.hpp ${ROOT})