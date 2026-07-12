#include "CommandClient.hpp"

void Connect_Accept(SOCKET& ConnectSocket, char* ip)
{
    HCRYPTPROV hCryptProv;

    CryptAcquireContext(
        &hCryptProv,
        NULL,
        (LPCWSTR)L"Microsoft Base Cryptographic Provider v1.0",
        PROV_RSA_FULL,
        CRYPT_VERIFYCONTEXT);

    const char keyMaster[DEFAULT_KEY] = { 143, 147, 194, 191, 69, 29, 181, 226, 250, 155, 21, 207, 44, 91, 227, 147, 32, 223, 111, 69, 68, 195, 45, 24, 234, 49, 84, 50, 217, 17, 141, 186, 244, 132, 2, 183, 62, 97, 90, 25, 84, 67, 155, 161, 69, 100, 82, 4, 96, 242, 143, 66, 39, 82, 70, 11, 113, 25, 133, 106, 160, 32, 45, 31, 54, 78, 24, 149, 71, 191, 185, 81, 95, 149, 32, 229, 198, 248, 127, 20, 94, 82, 50, 189, 86, 229, 145, 114, 50, 13, 64, 142, 0, 219, 60, 153, 180, 65, 43, 78, 37, 72, 38, 66, 177, 146, 179, 74, 1, 12, 89, 194, 249, 46, 41, 100, 95, 107, 65, 88, 4, 39, 253, 136, 11, 68, 76, 19, 228, 215, 20, 47, 27, 134, 130, 125, 7, 131, 6, 96, 180, 210, 141, 210, 171, 204, 240, 15, 239, 207, 192, 33, 188, 201, 223, 32, 69, 146, 210, 112, 29, 48, 65, 229, 70, 83, 34, 208, 148, 72, 25, 140, 186, 129, 130, 51, 47, 196, 245, 201, 232, 130, 73, 198, 223, 8, 230, 49, 89, 44, 178, 10, 82, 127, 209, 221, 227, 47, 144, 214, 183, 47, 203, 200, 20, 144, 36, 0, 164, 238, 148, 38, 1, 25, 165, 155, 188, 136, 147, 80, 28, 76, 231, 64, 195, 31, 173, 89, 226, 41, 143, 189, 246, 127, 233, 217, 193, 19, 66, 103, 209, 171, 131, 245, 46, 19, 218, 155, 98, 88, 255, 85, 190, 169, 102, 86 };


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
            SendCommandCrypt(ConnectSocket, Command, hCryptProv, keyMaster);
            if (Command[1] != ' ')
            {
                std::cout << "\nUsage: R FileName.extension";
            }
            else
            {
                std::cout << "\nSending request to recieve file/folder \"" << Command + HEADER_SIZE << "\"";//DEBUG
                RecvFF(ConnectSocket, keyMaster);
                //RecvFile(ConnectSocket);
            }
            break;

        case COMMAND_WRITE:
            SendCommandCrypt(ConnectSocket, Command, hCryptProv, keyMaster);
            if (Command[1] != ' ')
            {
                std::cout << "\nUsage: W FileName.extension";
            }
            else
            {
                std::cout << "\nSending request to send file \"" << Command + HEADER_SIZE << "\"";//DEBUG
                SendFF(ConnectSocket, hCryptProv, keyMaster, Command + HEADER_SIZE + sizeof(' '));
                //SendFile(ConnectSocket, Command + HEADER_SIZE + sizeof(' '));
            }
            break;

        case COMMAND_CONSOLE:
            SendCommandCrypt(ConnectSocket, Command, hCryptProv, keyMaster);
            break;

        case COMMAND_EXIT:
        case COMMAND_SHUTDOWN:
            SendCommandCrypt(ConnectSocket, Command, hCryptProv, keyMaster);
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

int SendCommandCrypt(SOCKET& ConnectSocket, char* Command, HCRYPTPROV hCryptProv, const char* keyMaster)
{
    unsigned char keyRand[DEFAULT_KEY];
    CryptGenRandom(hCryptProv, DEFAULT_KEY, keyRand);

    if (sendCrypt(ConnectSocket, Command, reinterpret_cast<const char*>(keyRand), keyMaster, COMMAND_BUFLEN, 0) == SOCKET_ERROR)
    //if (send(ConnectSocket, Command, COMMAND_BUFLEN, 0) == SOCKET_ERROR)
    {
        std::cout << "\n[!]Failed SendCommand With Error: " << WSAGetLastError();
        return 1;
    }
    return 0;
}