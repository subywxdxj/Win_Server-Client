#include "File.h"

void SocketError(SOCKET& Socket)//display error code and close the socket
{
    printf("\nsend failed with error: %d\n", WSAGetLastError());
    closesocket(Socket);
    WSACleanup();
}

void FShutDown(SOCKET& Socket)
{
    std::cout << "\nSHUTDOWN\n";
    closesocket(Socket);
    WSACleanup();
    exit(0);
}

bool RecvFF(SOCKET& Socket, const char keyMaster[DEFAULT_KEY])
{
    char FileFolderCode[HEADER_SIZE];
    int iResult = recv(Socket, FileFolderCode, HEADER_SIZE, 0);

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

bool SendFF(SOCKET& Socket, HCRYPTPROV& hCryptProv, const char keyMaster[DEFAULT_KEY], std::string Path)
{
    if (std::filesystem::is_directory(Path))
    {
        return SendFolder(Socket, hCryptProv, keyMaster, Path, true);
    }
    else if (std::filesystem::is_regular_file(Path))
    {
        return SendFile(Socket, hCryptProv, Path, keyMaster);
    }

    return false;
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
    std::cout << "\nRecieved Folder Name: " << FolderName << " [CREATE DIR]";//DEBUG

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

bool SendFolder(SOCKET& Socket, HCRYPTPROV& hCryptProv, const char keyMaster[DEFAULT_KEY], std::string Path, bool root)
{
    char SendFolderCode[HEADER_SIZE] = { SEND_FOLDER };
    send(Socket, SendFolderCode, HEADER_SIZE, 0);
    
    //send initial folder name
    PACKET sendPacket;
    sendPacket.buffer[0] = TYPE_INFO;
    std::memcpy(sendPacket.buffer + HEADER_SIZE, Path.c_str(), DEFAULT_DATA);

    unsigned char keyRand[DEFAULT_KEY];
    CryptGenRandom(hCryptProv, DEFAULT_KEY, keyRand);

    int iResult = sendCrypt(Socket, sendPacket.buffer, reinterpret_cast<const char*>(keyRand), keyMaster, DEFAULT_PACKET, 0);

    if (!iResult)
    {
        std::cout << "\n[!]Error Sending Folder Name";
    }


    for (const auto& entry : std::filesystem::directory_iterator(Path))
    {
        if (std::filesystem::is_directory(entry))//send folder data
        {
            std::cout << "\nFolder: " << entry.path().string();
            SendFolder(Socket, hCryptProv, keyMaster, entry.path().string(), false);
        }
        else if (std::filesystem::is_regular_file(entry))//call send file function
        {
            std::cout << "\n\nFile: " << entry.path().string();
            SendFile(Socket, hCryptProv, entry.path().string(), keyMaster);
        }
        else
        {
            std::cout << "\n[!]Error: ";
            return false;
        }
    }            
    
    if (root)
    {
        char SendFileHead[HEADER_SIZE] = { TRANSMIT_END };
        send(Socket, SendFileHead, HEADER_SIZE, 0);
    }

    return true;
}


bool SendFile(SOCKET& Socket, HCRYPTPROV& hCryptProv, std::string FileName, const char keyMaster[DEFAULT_KEY])
{
    char SendFileCode[HEADER_SIZE] = { SEND_FILE };
    send(Socket, SendFileCode, HEADER_SIZE, 0);

    std::ifstream DataFile(FileName, std::fstream::in | std::ios::binary);
    uint64_t FileSize;

    char FileStatus[HEADER_SIZE];

    if (DataFile.good())
    {
        FileSize = std::filesystem::file_size(FileName);
        std::cout << "\nFile \"" << FileName << "\" successfully open";
        FileStatus[0] = FILE_OPEN;
        send(Socket, FileStatus, HEADER_SIZE, 0);
    }
    else
    {
        std::cout << "\n[!]ERROR Opening file \"" << FileName << "\"";
        FileStatus[0] = FILE_OPEN_ERR;
        send(Socket, FileStatus, HEADER_SIZE, 0);
        return 0;
    }


    int remainder = FileSize % DEFAULT_DATA;
    int iSendResult;


    char sendPacket[DEFAULT_PACKET];
    sendPacket[0] = TYPE_INFO;//header file info

    std::memcpy(sendPacket + HEADER_SIZE, &FileSize, sizeof(uint64_t));
    std::memcpy(sendPacket + HEADER_SIZE + sizeof(FileSize), FileName.c_str(), (DEFAULT_PACKET - HEADER_SIZE - sizeof(FileSize)));

    unsigned char keyRand[DEFAULT_KEY];
    CryptGenRandom(hCryptProv, DEFAULT_KEY, keyRand);

    iSendResult = sendCrypt(Socket, sendPacket, reinterpret_cast<const char*>(keyRand), keyMaster, DEFAULT_PACKET, 0);
    if (iSendResult == SOCKET_ERROR)
    {
        SocketError(Socket);
        return 0;
    }

    sendPacket[0] = TYPE_DATA;//header data
    int ByteID = HEADER_SIZE;//skip header

    int BytesSent = 0;

    while (BytesSent + ByteID <= FileSize)
    {
        DataFile >> std::noskipws >> sendPacket[ByteID];
        ByteID++;
        if (ByteID == DEFAULT_DATA + HEADER_SIZE)
        {
            BytesSent += DEFAULT_DATA;
            ByteID = 1;

            CryptGenRandom(hCryptProv, DEFAULT_KEY, keyRand);
            iSendResult = sendCrypt(Socket, sendPacket, reinterpret_cast<const char*>(keyRand), keyMaster, DEFAULT_PACKET, 0);
            if (iSendResult == SOCKET_ERROR)
            {
                SocketError(Socket);
                DataFile.close();
                return 0;
            }
        }
    }
    DataFile.close();

    //Manually send remainder as last packet
    if (remainder)
    {
        sendPacket[0] = 1; 
        CryptGenRandom(hCryptProv, DEFAULT_KEY, keyRand);
        iSendResult = sendCrypt(Socket, sendPacket, reinterpret_cast<const char*>(keyRand), keyMaster, remainder + HEADER_SIZE, 0);
    }
    if (iSendResult == SOCKET_ERROR)
    {
        SocketError(Socket);
        return 0;
    }

    return 1;
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

    std::cout << "\nRecv File: " << FileName;//DEBUG


    std::ofstream SaveFile;
    int BytesRecieved = 0;
    SaveFile.open(FileName, std::ios::out | std::ios::binary);


    int remainder = FileSize % DEFAULT_DATA;

    do
    {
        if (BytesRecieved + DEFAULT_DATA + HEADER_SIZE >= FileSize)
        {
            recvPacket = recvCrypt(Socket, keyMaster, remainder + HEADER_SIZE, 0, iResult);
        }
        else
        {
            recvPacket = recvCrypt(Socket, keyMaster, DEFAULT_PACKET, 0, iResult);
        }
        if (iResult > 0)
        {
            BytesRecieved += iResult-1;
            for (int i = HEADER_SIZE; i < iResult; i++)
            {
                SaveFile << recvPacket[i];
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

    } while (iResult > 0 && recvPacket[0] == TYPE_DATA);


    SaveFile.close();
    return 1;
}