#pragma once
#include "Includes.h"

void SocketError(SOCKET& ClientSocket);//display error code and close the socket
void FShutDown(SOCKET Socket);


bool SendFile(SOCKET& ClientSocket, std::string FileName);
bool RecvFile(SOCKET& ConnectSocket);