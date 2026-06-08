#include "../File.h"

int Connect_Init(int argc, char** argv, SOCKET& ConnectSocket);

void Connect_Accept(SOCKET& ConnectSocket, char* ip);
int SendCommand(SOCKET& ConnectSocket, char* Command);
int SendCommandShutdown(SOCKET& ConnectSocket);