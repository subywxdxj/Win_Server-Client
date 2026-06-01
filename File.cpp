#include "File.h"

void SocketError(SOCKET &ClientSocket)//display error code and close the socket
{
    printf("\nsend failed with error: %d\n", WSAGetLastError());
    closesocket(ClientSocket);
    WSACleanup();
}

bool SendFile(SOCKET& ClientSocket, std::string FileName)
{
    std::cout << "\n\nOpening a file...";

    std::ifstream DataFile(FileName, std::fstream::in | std::ios::binary);
    uint64_t FileSize = std::filesystem::file_size(FileName);

    if (DataFile.good())
    {
        std::cout << "\nFile \"" << FileName << "\" successfully open";
    }
    else
    {
        std::cout << "\n[ERROR] Opening file \"" << FileName << "\"";
        return 0;
    }


    int remainder = FileSize % DEFAULT_DATA;
    int iSendResult;

    PACKET header;

    std::memcpy(header.buffer + sizeof(char), &FileSize, sizeof(uint64_t));
    std::memcpy(header.buffer + sizeof(char) + sizeof(uint64_t), &FileName, sizeof(char[255]));

    iSendResult = send(ClientSocket, header.buffer, sizeof(PACKET), 0);
    if (iSendResult == SOCKET_ERROR)
    {
        SocketError(ClientSocket);
        return 0;
    }
    //printf("\nBytes header sent: %d\n", iSendResult);//DEBUG

    PACKET sendPacket;
    sendPacket.buffer[0] = TYPE_DATA;//header
    int ByteID = HEADER_SIZE;//skip header

    int BytesSent = 0;

    while (BytesSent + ByteID < FileSize)
    {
        DataFile >> std::noskipws >> sendPacket.buffer[ByteID];
        ByteID++;
        if (ByteID == DEFAULT_DATA + HEADER_SIZE)
        {
            BytesSent += DEFAULT_DATA;
            ByteID = 1;

            iSendResult = send(ClientSocket, sendPacket.buffer, sizeof(PACKET), 0);
            if (iSendResult == SOCKET_ERROR)
            {
                SocketError(ClientSocket);
                DataFile.close();
                return 0;
            }
        }
    }
    DataFile.close();

    //Manually send remainder as last packet
    if (remainder)
    {
        iSendResult = send(ClientSocket, sendPacket.buffer, remainder, 0);
    }
    if (iSendResult == SOCKET_ERROR)
    {
        SocketError(ClientSocket);
        return 0;
    }

    //printf("\nBytes sent remainder: %d\n", iSendResult);//DEBUG

    return 1;
}



bool RecvFile(SOCKET &ConnectSocket)
{
    uint64_t FileSize;
    std::string FileName;

    PACKET recvPacket;

    PACKET header;
    int iResult = recv(ConnectSocket, header.buffer, sizeof(PACKET), 0);
    printf("Bytes header received: %d\n", iResult);

    std::memcpy(&FileSize, header.buffer + sizeof(char), sizeof(uint64_t));
    std::memcpy(&FileName, header.buffer + sizeof(char) + sizeof(uint64_t), sizeof(char[255]));

    std::cout << "\nFileSize: " << FileSize << "\nFileName: " << FileName << "\n";

    std::ofstream SaveFile;
    SaveFile.open(FileName, std::ios::out | std::ios::binary);

    // Receive until the peer closes the connection
    iResult = recv(ConnectSocket, recvPacket.buffer, sizeof(PACKET), 0);
    while (iResult > 0 && recvPacket.buffer[0] == TYPE_DATA)
    {
        if (iResult > 0)
        {
            //printf("Bytes received: %d\n", iResult);//DEBUG
            for (int i = HEADER_SIZE; i < iResult; i++)
            {
                SaveFile << recvPacket.buffer[i];
            }
        }
        else if (iResult == 0)
        {
            printf("Connection closed\n");
            SaveFile.close();
        }
        else
        {
            printf("recv failed with error: %d\n", WSAGetLastError()); 
            SaveFile.close();
            return 0;
        }

        iResult = recv(ConnectSocket, recvPacket.buffer, sizeof(PACKET), 0);
    }

    std::cout << "Recv End...";
    SaveFile.close();

    return 1;
}