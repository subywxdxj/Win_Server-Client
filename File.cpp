#include "File.h"

void SocketError(SOCKET &ClientSocket)//display error code and close the socket
{
    printf("\nsend failed with error: %d\n", WSAGetLastError());
    closesocket(ClientSocket);
    WSACleanup();
}

void FShutDown(SOCKET Socket)
{
    std::cout << "\nSHUTDOWN\n";
    closesocket(Socket);
    WSACleanup();
    exit(0);
}


bool SendFile(SOCKET& ClientSocket, std::string FileName)
{

    std::ifstream DataFile(FileName, std::fstream::in | std::ios::binary);
    uint64_t FileSize;

    char FileStatus[HEADER_SIZE];

    if (DataFile.good())
    {
        FileSize = std::filesystem::file_size(FileName);
        std::cout << "\nFile \"" << FileName << "\" successfully open";
        FileStatus[0] = FILE_OPEN;
        send(ClientSocket, FileStatus, HEADER_SIZE, 0);
    }
    else
    {
        std::cout << "\n[!]ERROR Opening file \"" << FileName << "\"";
        FileStatus[0] = FILE_OPEN_ERR;
        send(ClientSocket, FileStatus, HEADER_SIZE, 0);
        return 0;
    }


    int remainder = FileSize % DEFAULT_DATA;
    int iSendResult;


    PACKET sendPacket;
    sendPacket.buffer[0] = TYPE_INFO;//header file info

    std::memcpy(sendPacket.buffer + HEADER_SIZE, &FileSize, sizeof(uint64_t));
    std::memcpy(sendPacket.buffer + HEADER_SIZE + sizeof(FileSize), FileName.c_str(), sizeof(char[255]));

    iSendResult = send(ClientSocket, sendPacket.buffer, sizeof(PACKET), 0);
    if (iSendResult == SOCKET_ERROR)
    {
        SocketError(ClientSocket);
        return 0;
    }

    sendPacket.buffer[0] = TYPE_DATA;//header data
    int ByteID = HEADER_SIZE;//skip header

    int BytesSent = 0;

    while (BytesSent + ByteID <= FileSize)
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
        sendPacket.buffer[0] = 1;
        iSendResult = send(ClientSocket, sendPacket.buffer, remainder + HEADER_SIZE, 0);
    }
    if (iSendResult == SOCKET_ERROR)
    {
        SocketError(ClientSocket);
        return 0;
    }

    return 1;
}



bool RecvFile(SOCKET &ConnectSocket)
{
    uint64_t FileSize;
    std::string FileName;

    char FileStatus[HEADER_SIZE];

    int iResult = recv(ConnectSocket, FileStatus, HEADER_SIZE, 0);
    if (iResult && FileStatus[0] == FILE_OPEN_ERR)
    {
        std::cout << "\n[!]Server Error Opening file";
        return false;
    }


    PACKET recvPacket;

    iResult = recv(ConnectSocket, recvPacket.buffer, sizeof(PACKET), 0);

    if (recvPacket.buffer[0] != TYPE_INFO)
    {
        std::cout << "\n[!]EXPECTED TYPE_INFO";
        return false;
    }

    std::memcpy(&FileSize, recvPacket.buffer + HEADER_SIZE, sizeof(FileSize));
    char buff[255] = "\0";
    std::memcpy(buff, recvPacket.buffer + HEADER_SIZE + sizeof(FileSize), sizeof(char[255]));
    FileName = buff;


    std::ofstream SaveFile;
    int BytesRecieved = 0;
    SaveFile.open(FileName, std::ios::out | std::ios::binary);


    int remainder = FileSize % DEFAULT_DATA;
    do
    {
        if (BytesRecieved + DEFAULT_DATA + HEADER_SIZE >= FileSize)
        {
            std::cout << "\n";
            iResult = recv(ConnectSocket, recvPacket.buffer, remainder + HEADER_SIZE, 0);
        }
        else
        {
            iResult = recv(ConnectSocket, recvPacket.buffer, sizeof(PACKET), 0);
        }
        if (iResult > 0)
        {
            BytesRecieved += iResult-1;
            for (int i = HEADER_SIZE; i < iResult; i++)
            {
                SaveFile << recvPacket.buffer[i];
            }
        }
        else if (iResult == 0)
        {
            printf("Connection closed\n");
            SaveFile.close();
            return 0;
        }
        else
        {
            printf("recv failed with error: %d\n", WSAGetLastError()); 
            SaveFile.close();
            return 0;
        }

    } while (iResult > 0 && recvPacket.buffer[0] == TYPE_DATA);


    SaveFile.close();
    return 1;
}