#include "CommandClient.h"


int __cdecl main(int argc, char** argv)
{
    SOCKET ConnectSocket = INVALID_SOCKET;
    if (Client_Init(argc, argv, ConnectSocket))//1 = error; 0 = good
    {
        std::cout << "\n[!]Init Failed...";
        return 0;
    }

    while (true)
    {
        char Cnsl_Command[COMMAND_BUFLEN] = "B";//Shutdown
        std::cout << "\nInput command: ";

        std::cin.getline(Cnsl_Command, COMMAND_BUFLEN);

        SendCommandConsole(ConnectSocket, Cnsl_Command);
    }
    //SendCommandShutdown(ConnectSocket);

    //
    //
    //uint64_t FileSize;
    //std::string FileName;
    //
    //if (!RecvFile(ConnectSocket)) { return -1; }//error recieving file
    //
    //
    //// cleanup
    //closesocket(ConnectSocket);
    //WSACleanup();

    return 0;
}