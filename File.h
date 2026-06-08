#pragma once
#include "Includes.h"

void SocketError(SOCKET& Socket);//display error code and close the socket
void FShutDown(SOCKET& Socket);

bool RecvFF(SOCKET& Socket);
bool SendFF(SOCKET& Socket, std::string Path);

bool SendFile(SOCKET& Socket, std::string FileName);
bool SendFolder(SOCKET& Socket, std::string Path, bool root);

bool RecvFile(SOCKET& Socket);
bool RecvFolder(SOCKET& Socket);
std::string RecvFolderPath(SOCKET& Socket);