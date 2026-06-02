#pragma once
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>

#include <iostream>
#include <thread>
#include <fstream>
#include <filesystem>
#include <vector>


// Need to link with Ws2_32.lib, Mswsock.lib, and Advapi32.lib
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")

#define TRANSMIT_END 1
#define TYPE_DATA 0
#define TYPE_INFO 2
#define COMMAND_CONSOLE 64//'@'
#define COMMAND_SHUTDOWN 66//Execute order 66 'B'
#define COMMAND_EXIT 69//'E'

#define COMMAND_READ 82//'R'
#define COMMAND_WRITE 87//'W'


#define HEADER_SIZE 1

#define DEFAULT_DATA 511
#define COMMAND_BUFLEN 47
#define FILE_HEAD_BUFLEN sizeof(uint64_t)/*filesize*/ + sizeof(char[255])/*filename*/
#define DEFAULT_PORT "595"//microsoft looking port

struct PACKET
{
    char buffer[1/*type*/ + DEFAULT_DATA];
};