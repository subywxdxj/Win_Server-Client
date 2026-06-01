#pragma once
#include "Includes.h"

void SocketError(SOCKET& ClientSocket);//display error code and close the socket

bool SendFile(SOCKET& ClientSocket, std::string FileName);
bool RecvFile(SOCKET& ConnectSocket);