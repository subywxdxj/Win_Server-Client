#include "Send.hpp"


bool SendFF(SOCKET& Socket, HCRYPTPROV& hCryptProv, const char keyMaster[DEFAULT_KEY], std::string Path)
{
    if (FileExist(Path))
    {
        char FileStatus[HEADER_SIZE];
        FileStatus[0] = FILE_OPEN;
        send(Socket, FileStatus, HEADER_SIZE, 0);
        std::cout << "\nDATA Exists";
    }
    else
    {
        char FileStatus[HEADER_SIZE];
        FileStatus[0] = FILE_OPEN_ERR;
        send(Socket, FileStatus, HEADER_SIZE, 0);
        std::cout << "\n[!]ERROR DATA DOES NOT Exist";
        return false;
    }

    if (std::filesystem::is_directory(Path))
    {            
        bool res = SendFolder(Socket, hCryptProv, keyMaster, Path);
        char SendFileHead[HEADER_SIZE] = { TRANSMIT_END };
        send(Socket, SendFileHead, HEADER_SIZE, 0);
        return res;
    }
    else if (std::filesystem::is_regular_file(Path))
    {
        return SendFile(Socket, hCryptProv, Path, keyMaster);
    }

    return true;
}



bool SendFolder(SOCKET& Socket, HCRYPTPROV& hCryptProv, const char keyMaster[DEFAULT_KEY], std::string Path)
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
            SendFolder(Socket, hCryptProv, keyMaster, entry.path().string());
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

    return true;
}


bool SendFile(SOCKET& Socket, HCRYPTPROV& hCryptProv, std::string FileName, const char keyMaster[DEFAULT_KEY])
{
    char SendFileCode[HEADER_SIZE] = { SEND_FILE };
    send(Socket, SendFileCode, HEADER_SIZE, 0);
    //std::cout << "\nSent SendFileCode 1B";//DEBUG

    std::ifstream DataFile(FileName, std::fstream::in | std::ios::binary);
    uint64_t FileSize;

    char FileStatus[HEADER_SIZE];

    if (DataFile.good())
    {
        FileSize = std::filesystem::file_size(FileName);
        std::cout << "\nFile \"" << FileName << "\" successfully open";
        FileStatus[0] = FILE_OPEN;
        send(Socket, FileStatus, HEADER_SIZE, 0);
        //std::cout << "\nSent FileStatus 1B";//DEBUG
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

    //char Packet[DEFAULT_DATA];
    sendPacket[0] = TYPE_DATA;//header data
    int ByteID = HEADER_SIZE;//skip header

    int BytesSent = 0;

    while (BytesSent + ByteID - 1 < FileSize)
    {
        DataFile >> std::noskipws >> sendPacket[ByteID];
        ByteID++;
        if (ByteID == DEFAULT_PACKET)
        {
            BytesSent += DEFAULT_DATA;
            ByteID = 1;

            if (BytesSent == FileSize)
            {
                sendPacket[0] = TRANSMIT_END;
            }

            CryptGenRandom(hCryptProv, DEFAULT_KEY, keyRand);
            iSendResult = sendCrypt(Socket, sendPacket, reinterpret_cast<const char*>(keyRand), keyMaster, DEFAULT_PACKET, 0);

            //std::cout << "\nSent " << iSendResult << "B (" << BytesSent << "/" << FileSize << ") [" << (int)sendPacket[0] << "]";//DEBUG

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
        sendPacket[0] = TRANSMIT_END;
        CryptGenRandom(hCryptProv, DEFAULT_KEY, keyRand);
        iSendResult = sendCrypt(Socket, sendPacket, reinterpret_cast<const char*>(keyRand), keyMaster, remainder + HEADER_SIZE, 0);

        std::cout << "\nSent " << iSendResult << "B (" << BytesSent << "/" << FileSize << ") [" << (int)sendPacket[0] << "]";//DEBUG
    }
    if (iSendResult == SOCKET_ERROR)
    {
        SocketError(Socket);
        return 0;
    }

    return 1;
}