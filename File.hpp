#pragma once
#include "Crypt.hpp"

void SocketError(SOCKET& Socket);//display error code and close the socket
void FShutDown(SOCKET& Socket);

bool FileExist(const std::string& name);