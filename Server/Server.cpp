#include "CommandServer.h"


int __cdecl main(void)
{
    SOCKET ClientSocket = INVALID_SOCKET;
    SOCKET ListenSocket = INVALID_SOCKET;

    if (int err = Server_Init(ClientSocket, ListenSocket))//1 = error; 0 = good
    {
        std::cout << "\n[!]Init Failed With ERROR " << err;
        return 0;
    }


    int iResult = 1;

    while (true)
    {
        // Accept a client socket
        ClientSocket = accept(ListenSocket, NULL, NULL);
        if (ClientSocket == INVALID_SOCKET)
        {
            printf("[!]Accept Failed With Error: %d\n", WSAGetLastError());
            closesocket(ListenSocket);
            WSACleanup();
            return 1;
        }
        else
        {
          printf("ACCEPT CONNECTION\n");
        }


        char recvbuf[COMMAND_BUFLEN];
        int recvbuflen = COMMAND_BUFLEN;

        // Receive until the peer shuts down the connection
        while (iResult > 0)
        {
            do
            {
                iResult = recv(ClientSocket, recvbuf, recvbuflen, 0);
            } while (iResult <= 0);

            switch (recvbuf[0])
            {
            case COMMAND_READ:
                std::cout << "\nSending file \"" << recvbuf + sizeof(char) * HEADER_SIZE + sizeof(' ') << "\"";//DEBUG
                SendFile(ClientSocket, recvbuf + sizeof(char) * HEADER_SIZE + sizeof(' '));
                break;

            case COMMAND_WRITE:
                std::cout << "\nRecieving a file";//DEBUG
                RecvFile(ClientSocket);
                break;

            case COMMAND_CONSOLE:
                ExecuteCommandConsole(recvbuf);
                break;

            case COMMAND_EXIT:
                std::cout << "\n[:]EXIT Command Recieved!";
                closesocket(ClientSocket);
                WSACleanup();
                iResult = 0;//exit communication loop
                break;

            case COMMAND_SHUTDOWN:
                std::cout << "\n[:]SHUTDOWN Command Recieved!";
                ShutDown(ClientSocket);
                break;
            }
        }
    }

    return 0;
}