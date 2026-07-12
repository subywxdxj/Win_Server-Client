#include "File.hpp"

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

bool FileExist(const std::string& name) 
{
    struct stat buffer;
    return (stat(name.c_str(), &buffer) == 0);
}