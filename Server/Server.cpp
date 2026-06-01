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

    std::thread t1(ShutDownOnBind, ListenSocket);//manual server shutdown on bind


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
            iResult = recv(ClientSocket, recvbuf, recvbuflen, 0);

            switch (recvbuf[0])
            {
            case COMMAND_SHUTDOWN:
                ShutDown(ClientSocket);
                break;

            case COMMAND_CONSOLE:
                ExecuteCommandConsole(recvbuf);
                break;
            }




            //if (iResult > 0)
            //{
            //    printf("Bytes received: %d\n\"", iResult);
            //
            //    for (int i = 0; i < iResult; i++)
            //    {
            //        std::cout << recvbuf[i];
            //    }
            //    std::cout << "\"\nInput file_name: ";
            //
            //    std::string fileName = "hitla.mp4";//"Smiling.mkv"
            //    //std::cin >> fileName;
            //
            //    if (!SendFile(ClientSocket, fileName)) { return 1; }//send file failed
            //}
            //else if (iResult == 0)
            //    printf("\nConnection closed\n");
            //else
            //{
            //    printf("[!]Recv Failed With Error: %d\n", WSAGetLastError());
            //    closesocket(ClientSocket);
            //    WSACleanup();
            //    return 1;
            //}

        }



        // shutdown the connection since we're done
        iResult = shutdown(ClientSocket, SD_SEND);
        if (iResult == SOCKET_ERROR)
        {
            printf("[!]Shutdown Failed With Error: %d\n", WSAGetLastError());
            closesocket(ClientSocket);
            WSACleanup();
            return 1;
        }



        // cleanup
        closesocket(ClientSocket);
    }


    WSACleanup();
    return 0;
}