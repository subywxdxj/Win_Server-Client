#include "CommandServer.h"
#include <thread>

int __cdecl main(void)
{
    bool exit = false;

    SOCKET ListenSocket = INVALID_SOCKET;

    if (int err = Server_Init(ListenSocket))//1 = error; 0 = good
    {
        std::cout << "\n[!]Init Failed With ERROR " << err;
        return 0;
    }

    DWORD dwThreadIDs[MAX_CONNECTIONS];
    HANDLE hThreads[MAX_CONNECTIONS];

    Client_Data Data[MAX_CONNECTIONS + MAX_QUEUE];//queue is only sending decline connection reasons and disconnects for now

    int ThreadsCount = 0;

    while (true)
    {
        SOCKET ClientSocket = INVALID_SOCKET;

        sockaddr_in ClientAddr;
        int ClientAddrSz = sizeof(ClientAddr);
        ClientAddr.sin_family = AF_INET;
        // Accept a client socket
        ClientSocket = accept(ListenSocket, (SOCKADDR*)&ClientAddr, &ClientAddrSz);

        if (ClientSocket == INVALID_SOCKET)
        {
            printf("[!]Accept Failed With Error: %d\n", WSAGetLastError());
            closesocket(ListenSocket);
            WSACleanup();
            return 1;
        }


        char ip[10];
        inet_ntop(ClientAddr.sin_family, &ClientAddr.sin_addr, ip, sizeof(ip));
        int port = htons(ClientAddr.sin_port);

        int ThreadID;
        for (ThreadID = 0; ThreadID < MAX_CONNECTIONS; ThreadID++)//create a communication thread
        {
            if (Data[ThreadID].status == false)//open slot
            {
                std::cout << "ACCEPT CONNECTION N." << ThreadID
                    << "\nIP." << ip << ":" << port << "\n";

                Data[ThreadID].ClientSocket = ClientSocket;
                Data[ThreadID].ClientAddr = ClientAddr;
                Data[ThreadID].reason = REASON_NONE;
                Data[ThreadID].ThreadID = ThreadID;

                hThreads[ThreadID] = CreateThread
                (
                    NULL,	//Security attributes
                    0,		//Stack size
                    (LPTHREAD_START_ROUTINE)Client_Accept, //Communication loop
                    &Data[ThreadID],
                    0,
                    &dwThreadIDs[ThreadID]
                );
                std::cout << "Thread: " << dwThreadIDs[ThreadID] << "\n\n";
                break;
            }
        }

        if (ThreadID == MAX_CONNECTIONS)//decline communication
        {
            for (; ThreadID < MAX_CONNECTIONS + MAX_QUEUE; ThreadID++)
            {
                if (Data[ThreadID].status == false)
                {
                    std::cout << "DECLINE CONNECTION N." << ThreadID
                        << "\nIP." << ip << ":" << port << "\n";

                    Data[ThreadID].ClientSocket = ClientSocket;
                    Data[ThreadID].ClientAddr = ClientAddr;
                    Data[ThreadID].reason = REASON_MAX_CONNECTIONS;
                    Data[ThreadID].ThreadID = ThreadID;

                    hThreads[ThreadID] = CreateThread
                    (
                        NULL,	//Security attributes
                        0,		//Stack size
                        (LPTHREAD_START_ROUTINE)Client_Decline,
                        &Data[ThreadID],
                        0,
                        &dwThreadIDs[ThreadID]
                    );
                    break;
                }
            }
        }
    }

    return 0;
}