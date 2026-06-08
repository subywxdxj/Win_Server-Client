#include "CommandClient.h"

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

    Connect_Accept(ConnectSocket, argv[1]);//comms loop

    return 0;
}