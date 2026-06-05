#include "CommandClient.h"

//add connection timeout for improper client disconnect
//add folder parsing and other folder/path to file related stuff for R/W commands

int __cdecl main(int argc, char** argv)
{
    if (argc == 1)//DEBUG
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
                std::cout << "\nSending request to recieve file \"" << Command + HEADER_SIZE << "\"";//DEBUG
                RecvFile(ConnectSocket);
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
                SendFile(ConnectSocket, Command + HEADER_SIZE + sizeof(' '));
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

    return 0;
}