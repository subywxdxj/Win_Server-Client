#include "../File.h"

int Server_Init(SOCKET& ClientSocket, SOCKET& ListenSocket);
void ShutDown(SOCKET ListenSocket);

int ExecuteCommandConsole(char* CommandBuf);
void ShutDownOnBind(SOCKET ListenSocket);//DEBUG
