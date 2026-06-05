#include "../File.h"
#define MAX_CONNECTIONS 2
#define MAX_QUEUE 2

int Server_Init(SOCKET& ListenSocket);
int ShutDown(SOCKET& ClientSocket);

void Client_Accept(Client_Data& Data);
void Client_Decline(Client_Data& Data);

int ExecuteCommandConsole(char* CommandBuf);