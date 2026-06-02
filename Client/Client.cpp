#include "CommandClient.h"


int __cdecl main(int argc, char** argv)
{
    if (argc == 1)
    {
        char def[10];
        std::memcpy(def, "localhost", sizeof("localhost"));
        argc++;
        argv[1] = def;
    }
    SOCKET ConnectSocket = INVALID_SOCKET;
    if (Connect_Init(argc, argv, ConnectSocket))//1 = error; 0 = good
    {
        std::cout << "\n[!]Init Failed...";
        return 0;
    }

    bool exit = false;
    while (!exit)
    {
        char Command[COMMAND_BUFLEN];
        std::cout << "\n" << argv[1] << ": ";

        std::cin.getline(Command, COMMAND_BUFLEN);


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
                std::cout << "\nSending request to recieve file \"" << Command + sizeof(char) * HEADER_SIZE << "\"";//DEBUG
                RecvFile(ConnectSocket);
            }
            break;

        case COMMAND_WRITE:
            SendCommand(ConnectSocket, Command);
            if (Command[1] != ' ')
            {
                std::cout << "\nUsage: R FileName.extension";
            }
            else
            {
                std::cout << "\nSending request to send file \"" << Command + sizeof(char) * HEADER_SIZE << "\"";//DEBUG
                SendFile(ConnectSocket, Command + sizeof(char) * HEADER_SIZE) + sizeof(' ');
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
        }
        
    }

    return 0;
}