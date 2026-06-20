#include "CommandServer.h"

void Client_Accept(Client_Data& Data)
{
    HCRYPTPROV hCryptProv;

    CryptAcquireContext(
        &hCryptProv,
        NULL,
        (LPCWSTR)L"Microsoft Base Cryptographic Provider v1.0",
        PROV_RSA_FULL,
        CRYPT_VERIFYCONTEXT);

    const char keyMaster[DEFAULT_KEY] = { 143, 147, 194, 191, 69, 29, 181, 226, 250, 155, 21, 207, 44, 91, 227, 147, 32, 223, 111, 69, 68, 195, 45, 24, 234, 49, 84, 50, 217, 17, 141, 186, 244, 132, 2, 183, 62, 97, 90, 25, 84, 67, 155, 161, 69, 100, 82, 4, 96, 242, 143, 66, 39, 82, 70, 11, 113, 25, 133, 106, 160, 32, 45, 31, 54, 78, 24, 149, 71, 191, 185, 81, 95, 149, 32, 229, 198, 248, 127, 20, 94, 82, 50, 189, 86, 229, 145, 114, 50, 13, 64, 142, 0, 219, 60, 153, 180, 65, 43, 78, 37, 72, 38, 66, 177, 146, 179, 74, 1, 12, 89, 194, 249, 46, 41, 100, 95, 107, 65, 88, 4, 39, 253, 136, 11, 68, 76, 19, 228, 215, 20, 47, 27, 134, 130, 125, 7, 131, 6, 96, 180, 210, 141, 210, 171, 204, 240, 15, 239, 207, 192, 33, 188, 201, 223, 32, 69, 146, 210, 112, 29, 48, 65, 229, 70, 83, 34, 208, 148, 72, 25, 140, 186, 129, 130, 51, 47, 196, 245, 201, 232, 130, 73, 198, 223, 8, 230, 49, 89, 44, 178, 10, 82, 127, 209, 221, 227, 47, 144, 214, 183, 47, 203, 200, 20, 144, 36, 0, 164, 238, 148, 38, 1, 25, 165, 155, 188, 136, 147, 80, 28, 76, 231, 64, 195, 31, 173, 89, 226, 41, 143, 189, 246, 127, 233, 217, 193, 19, 66, 103, 209, 171, 131, 245, 46, 19, 218, 155, 98, 88, 255, 85, 190, 169, 102, 86 };


    char answr[COMMAND_BUFLEN];

    answr[0] = Data.reason;
    send(Data.ClientSocket, answr, HEADER_SIZE, 0);//send accept code
    Data.status = true;

    // Receive until the peer shuts down the connection
    int iResult = 1;
    while (iResult > 0)
    {
        char* recvbuf = recvCrypt(Data.ClientSocket, keyMaster, COMMAND_BUFLEN, 0, iResult);//recieve command

        switch (recvbuf[0])
        {
        case COMMAND_READ:
            std::cout << "\nSending file \"" << recvbuf + sizeof(char) * HEADER_SIZE + sizeof(' ') << "\"";//DEBUG
            SendFF(Data.ClientSocket, hCryptProv, keyMaster, recvbuf + sizeof(char) * HEADER_SIZE + sizeof(' '));
            //SendFile(Data.ClientSocket, recvbuf + sizeof(char) * HEADER_SIZE + sizeof(' '));
            break;

        case COMMAND_WRITE:
            std::cout << "\nRecieving a file/folder";//DEBUG
            RecvFF(Data.ClientSocket, keyMaster);
            //RecvFile(Data.ClientSocket);
            break;

        case COMMAND_CONSOLE:
            ExecuteCommandConsole(recvbuf);
            break;

        case COMMAND_EXIT:
            std::cout << "\n[+]EXIT Command Recieved!\n";
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