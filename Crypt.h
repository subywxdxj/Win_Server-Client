#include "Includes.h"

char* Crypt(const char* data, const char* key, int size);
const char* CryptPacket(const char* data, const char keyRand[DEFAULT_KEY], const char keyMaster[DEFAULT_KEY], int size);

int sendCrypt(SOCKET& s, const char* data, const char keyRand[DEFAULT_KEY], const char keyMaster[DEFAULT_KEY], int size, int flags);
char* recvCrypt(SOCKET& s, const char* keyMaster, int size, int flags, int& result);

void DebugChar(char* buf, int size);