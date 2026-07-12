#include "Recv.hpp"

bool RecvFF(SOCKET& Socket, const char keyMaster[DEFAULT_KEY])
{
    char FileFolderCode[HEADER_SIZE];
    int iResult = recv(Socket, FileFolderCode, HEADER_SIZE, 0);
    if (FileFolderCode[0] != FILE_OPEN)
    {
        std::cout << "\n[!]NAME: No such File or Direcory";
        return false;
    }

    iResult = recv(Socket, FileFolderCode, HEADER_SIZE, 0);

    if (iResult && FileFolderCode[0] == SEND_FILE)
    {
        RecvFile(Socket, keyMaster);
    }
    else if (iResult && FileFolderCode[0] == SEND_FOLDER)
    {
        RecvFolder(Socket, keyMaster);
    }
    else
    {
        return false;
    }
    return true;
}

std::string RecvFolderPath(SOCKET& Socket, const char keyMaster[DEFAULT_KEY])
{
    int iResult = 0;
    char* recvPacket = recvCrypt(Socket, keyMaster, DEFAULT_PACKET, 0, iResult);

    if (iResult && recvPacket[0] != TYPE_INFO)
    {
        std::cout << "\n[!]EXPECTED TYPE_INFO (Folder)";
        return "Err";
    }

    char buff[255] = "\0";
    std::memcpy(buff, recvPacket + HEADER_SIZE, DEFAULT_DATA);
    std::string FolderName = buff;
    //std::cout << "\nRecieved Folder Name: " << FolderName << " [CREATE DIR]";//DEBUG

    return FolderName;
}

bool RecvFolder(SOCKET& Socket, const char keyMaster[DEFAULT_KEY])
{
    char FileFolderCode[HEADER_SIZE] = { SEND_FOLDER };//initial folder

    while (FileFolderCode[0] != TRANSMIT_END)
    {
        if (FileFolderCode[0] == SEND_FOLDER)
        {
            std::filesystem::create_directory(RecvFolderPath(Socket, keyMaster));
        }
        else if (FileFolderCode[0] == SEND_FILE)
        {
            RecvFile(Socket, keyMaster);
        }


        int iResult = recv(Socket, FileFolderCode, HEADER_SIZE, 0);
        if (!iResult)
        {
            std::cout << "\n[!]Error recieving File/Folder Code.";
            return false;
        }
    }
    return true;
}




bool RecvFile(SOCKET& Socket, const char keyMaster[DEFAULT_KEY])
{
    uint64_t FileSize;
    std::string FileName;

    char FileStatus[HEADER_SIZE];

    int iResult = recv(Socket, FileStatus, HEADER_SIZE, 0);
    if (iResult && FileStatus[0] == FILE_OPEN_ERR)
    {
        std::cout << "\n[!]Error Opening file";
        return false;
    }


    char* recvPacket;

    recvPacket = recvCrypt(Socket, keyMaster, DEFAULT_PACKET, 0, iResult);

    if (recvPacket[0] != TYPE_INFO)
    {
        std::cout << "\n[!]EXPECTED TYPE_INFO (File)";
        return false;
    }

    std::memcpy(&FileSize, recvPacket + HEADER_SIZE, sizeof(FileSize));
    char buff[247] = "\0";//(DEFAULT_PACKET - HEADER_SIZE - sizeof(FileSize)) = 247
    std::memcpy(buff, recvPacket + HEADER_SIZE + sizeof(FileSize), (DEFAULT_PACKET - HEADER_SIZE - sizeof(FileSize)));
    FileName = buff;

    //std::cout << "\nRecv File: " << FileName << " | Size: " << FileSize;//DEBUG


    std::ofstream SaveFile;
    int BytesRecieved = 0;
    SaveFile.open(FileName, std::ios::out | std::ios::binary);


    int remainder = FileSize % DEFAULT_DATA;

    if (FileSize > 0)
    {
        do
        {
            if (BytesRecieved + DEFAULT_DATA > FileSize && remainder)
            {
                recvPacket = recvCrypt(Socket, keyMaster, remainder + HEADER_SIZE, 0, iResult);
            }
            else
            {
                recvPacket = recvCrypt(Socket, keyMaster, DEFAULT_PACKET, 0, iResult);
            }
            if (iResult > 0)
            {
                BytesRecieved += iResult - 1;
                for (int i = HEADER_SIZE; i < iResult; i++)
                {
                    SaveFile << recvPacket[i];
                }
                //std::cout << "\nRecv " << iResult << "B (" << BytesRecieved << "/" << FileSize << ") [" << (int)recvPacket[0] << "]" << " [" << (int)recvPacket[1] << "]";//DEBUG
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

        } while (iResult > 0 && recvPacket[0] == TYPE_DATA);
    }


    SaveFile.close();
    return 1;
}