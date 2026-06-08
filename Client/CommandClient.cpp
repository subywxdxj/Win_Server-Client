#include "CommandClient.h"

void Connect_Accept(SOCKET& ConnectSocket, char* ip)
{
    bool exit = false;
    while (!exit)
    {
        char Command[COMMAND_BUFLEN];
        std::cout << "\n" << ip << ": ";

        std::cin.getline(Command, COMMAND_BUFLEN);

        int iResult;

        switch (Command[0])
        {
        case COMMAND_READ:
            SendCommand(ConnectSocket, Command);
            if (Command[1] != ' ')
            {
                std::cout << "\nUsage: R FileName.extension";
            }
            else
            {
                std::cout << "\nSending request to recieve file/folder \"" << Command + HEADER_SIZE << "\"";//DEBUG
                RecvFF(ConnectSocket);
                //RecvFile(ConnectSocket);
            }
            break;

        case COMMAND_WRITE:
            SendCommand(ConnectSocket, Command);
            if (Command[1] != ' ')
            {
                std::cout << "\nUsage: W FileName.extension";
            }
            else
            {
                std::cout << "\nSending request to send file \"" << Command + HEADER_SIZE << "\"";//DEBUG
                SendFF(ConnectSocket, Command + HEADER_SIZE + sizeof(' '));
                //SendFile(ConnectSocket, Command + HEADER_SIZE + sizeof(' '));
            }
            break;

        case COMMAND_CONSOLE:
            SendCommand(ConnectSocket, Command);
            break;

        case COMMAND_EXIT:
        case COMMAND_SHUTDOWN:
            SendCommand(ConnectSocket, Command);
            exit = true;//close client
            closesocket(ConnectSocket);
            WSACleanup();
            break;
        default:
            std::cout << "\nUsage:\nR FileName.extension //reads specified file from Server and sends it to Client"
                << "\nW FileName.extension //reads specified file from Client and sends it to Server"
                << "\n@echo test //@ followed by windows command that will be executed on the Server"
                << "\nE //exit communication with server and Shutdown Client"
                << "\nB //Shutdown the Server and Client\n";
        }
    }
    return;
}

int Connect_Init(int argc, char** argv, SOCKET& ConnectSocket)
{
    WSADATA wsaData;
    struct addrinfo* result = NULL,
        * ptr = NULL,
        hints;
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
        else
        {
            break;
        }
    }

    freeaddrinfo(result);

    if (ConnectSocket == INVALID_SOCKET) 
    {
        printf("[!]Unable to connect to server!\n");
        WSACleanup();
        return 1;
    }

    char ConnectionStatus[HEADER_SIZE];
    iResult = recv(ConnectSocket, ConnectionStatus, HEADER_SIZE, 0);
    if (iResult && ConnectionStatus[0] != REASON_NONE)
    {
        std::cout << "\n[!]Connection Declined with error: " << (int)ConnectionStatus[0];
        WSACleanup();
        return 1;
    }
    std::cout << "\n[+]Connection Accepted";

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

int SendCommand(SOCKET& ConnectSocket, char* Command)
{
    if (send(ConnectSocket, Command, COMMAND_BUFLEN, 0) == SOCKET_ERROR)
    {
        std::cout << "\n[!]Failed SendCommand With Error: " << WSAGetLastError();
        return 1;
    }
    return 0;
}