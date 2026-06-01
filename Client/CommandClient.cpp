#include "CommandClient.h"

int Client_Init(int argc, char** argv, SOCKET& ConnectSocket)
{
    WSADATA wsaData;
    struct addrinfo* result = NULL,
        * ptr = NULL,
        hints;
    const char* sendbuf = "connection established";
    int iResult;

    // Validate the parameters
    if (argc != 2) 
    {
        printf("usage: %s server-name\n", argv[0]);
        return 1;
    }

    // Initialize Winsock
    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) 
    {
        printf("[!]WSAStartup Failed With Error: %d\n", iResult);
        return 1;
    }

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    // Resolve the server address and port
    iResult = getaddrinfo(argv[1], DEFAULT_PORT, &hints, &result);
    if (iResult != 0) 
    {
        printf("[!]Getaddrinfo Failed With Error: %d\n", iResult);
        WSACleanup();
        return 1;
    }

    // Attempt to connect to an address until one succeeds
    for (ptr = result; ptr != NULL; ptr = ptr->ai_next) 
    {

        // Create a SOCKET for connecting to server
        ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype,
            ptr->ai_protocol);
        if (ConnectSocket == INVALID_SOCKET) {
            printf("[!]Socket Failed With Error: %ld\n", WSAGetLastError());
            WSACleanup();
            return 1;
        }

        // Connect to server.
        iResult = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
        if (iResult == SOCKET_ERROR) 
        {
            closesocket(ConnectSocket);
            ConnectSocket = INVALID_SOCKET;
            continue;
        }
        break;
    }

    freeaddrinfo(result);

    if (ConnectSocket == INVALID_SOCKET) 
    {
        printf("Unable to connect to server!\n");
        WSACleanup();
        return 1;
    }

    // Send an initial buffer
    //iResult = send(ConnectSocket, sendbuf, (int)strlen(sendbuf), 0);
    //if (iResult == SOCKET_ERROR) 
    //{
    //    printf("[!]Send Failed With Error: %d\n", WSAGetLastError());
    //    closesocket(ConnectSocket);
    //    WSACleanup();
    //    return 1;
    //}
    //
    //printf("Bytes Sent: %ld\n", iResult);

    // shutdown the connection since no more data will be sent
    //iResult = shutdown(ConnectSocket, SD_SEND);
    //if (iResult == SOCKET_ERROR) 
    //{
    //    printf("[!]Shutdown Failed With Error: %d\n", WSAGetLastError());
    //    closesocket(ConnectSocket);
    //    WSACleanup();
    //    return 1;
    //}
    return 0;
}

int SendCommandShutdown(SOCKET& ConnectSocket)
{
    char Buffer[COMMAND_BUFLEN];
    Buffer[0] = COMMAND_SHUTDOWN;
    if (send(ConnectSocket, Buffer, COMMAND_BUFLEN, 0) == SOCKET_ERROR)
    {
        std::cout << "\n[!]Failed SendConsoleCommand With Error: " << WSAGetLastError();
        return 1;
    }
    return 0;
}

int SendCommandConsole(SOCKET& ConnectSocket, char* Command)
{
    //char Buffer[COMMAND_BUFLEN];
    //Buffer[0] = COMMAND_CONSOLE;
    //std::memcpy(Buffer + sizeof(char) * HEADER_SIZE, Command, sizeof(Command) - sizeof(char) * HEADER_SIZE);
    //
    //std::cout << "Command Got: " << Command;
    //std::cout << "Command Sent: " << Buffer;

    if (send(ConnectSocket, Command, COMMAND_BUFLEN, 0) == SOCKET_ERROR)
    {
        std::cout << "\n[!]Failed SendConsoleCommand With Error: " << WSAGetLastError();
        return 1;
    }
    return 0;
}