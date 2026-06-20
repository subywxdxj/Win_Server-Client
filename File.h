#pragma once
#include "Crypt.h"

void SocketError(SOCKET& Socket);//display error code and close the socket
void FShutDown(SOCKET& Socket);

bool RecvFF(SOCKET& Socket, const char keyMaster[DEFAULT_KEY]);
bool SendFF(SOCKET& Socket, HCRYPTPROV& hCryptProv, const char keyMaster[DEFAULT_KEY], std::string Path);

bool SendFile(SOCKET& Socket, HCRYPTPROV& hCryptProv, std::string FileName, const char keyMaster[DEFAULT_KEY]);
bool SendFolder(SOCKET& Socket, HCRYPTPROV& hCryptProv, const char keyMaster[DEFAULT_KEY], std::string Path, bool root);

bool RecvFile(SOCKET& Socket, const char keyMaster[DEFAULT_KEY]);
bool RecvFolder(SOCKET& Socket, const char keyMaster[DEFAULT_KEY]);
std::string RecvFolderPath(SOCKET& Socket, const char keyMaster[DEFAULT_KEY]);