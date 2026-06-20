#include "../File.h"

int Connect_Init(int argc, char** argv, SOCKET& ConnectSocket);

void Connect_Accept(SOCKET& ConnectSocket, char* ip);
int SendCommandCrypt(SOCKET& ConnectSocket, char* Command, HCRYPTPROV hCryptProv, const char* keyMaster);
int SendCommandShutdown(SOCKET& ConnectSocket);