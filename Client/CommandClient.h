#include "../File.h"

int Connect_Init(int argc, char** argv, SOCKET& ConnectSocket);

int SendCommand(SOCKET& ConnectSocket, char* Command);
int SendCommandShutdown(SOCKET& ConnectSocket);