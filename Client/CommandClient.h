#include "../File.h"

int Client_Init(int argc, char** argv, SOCKET& ConnectSocket);
int SendCommandConsole(SOCKET& ConnectSocket, char* Command);
int SendCommandShutdown(SOCKET& ConnectSocket);