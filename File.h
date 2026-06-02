#pragma once
#include "Includes.h"

void SocketError(SOCKET& ClientSocket);//display error code and close the socket
void ShutDown(SOCKET Socket);


bool SendFile(SOCKET& ClientSocket, std::string FileName);
bool RecvFile(SOCKET& ConnectSocket);