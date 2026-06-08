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

#define TRANSMIT_END 111
#define TYPE_DATA 0
#define TYPE_INFO 2
#define COMMAND_CONSOLE 64//'@'
#define COMMAND_SHUTDOWN 66//Execute order 66 'B'
#define COMMAND_EXIT 69//'E'

#define COMMAND_READ 82//'R'
#define COMMAND_WRITE 87//'W'


#define REASON_NONE 0 //Accept connection
#define REASON_MAX_CONNECTIONS 1

#define FILE_OPEN 0 //File Successfully open
#define FILE_OPEN_ERR 1


#define HEADER_SIZE 1

#define SEND_FILE 11
#define SEND_FOLDER 12

#define DEFAULT_DATA 511
#define COMMAND_BUFLEN 47
#define FILE_HEAD_BUFLEN sizeof(uint64_t)/*filesize*/ + sizeof(char[255])/*filename*/
#define DEFAULT_PORT "595"//port

struct Client_Data
{
    SOCKET ClientSocket;
    sockaddr_in ClientAddr;

    int reason;//0 = accept connection, 1+ = error code
    int ThreadID;
    bool status = false;//status of connection to this client
};

struct PACKET
{
    char buffer[1/*type*/ + DEFAULT_DATA];
};