#include "CommandServer.h"

void ShutDownOnBind(SOCKET ListenSocket)//DEBUG
{
    while (!GetAsyncKeyState(VK_F12))
    {
        Sleep(1);
    }
    ShutDown(ListenSocket);

}

void ShutDown(SOCKET ListenSocket)
{
    std::cout << "\nSERVER CLOSED.\n";
    closesocket(ListenSocket);
    WSACleanup();
    exit(0);
}

int Server_Init(SOCKET& ClientSocket, SOCKET& ListenSocket)
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
    //char Command[COMMAND_BUFLEN] = { 0 };
    //
    //for (int i = HEADER_SIZE; i < COMMAND_BUFLEN; i++)
    //{
    //    Command[i - HEADER_SIZE] = CommandBuf[i];
    //    std::cout << " " << (int)CommandBuf[i];
    //}

    std::cout << "\nSystem: " << CommandBuf + sizeof(char) * HEADER_SIZE;
    system(CommandBuf + sizeof(char) * HEADER_SIZE);
    return 0;
}