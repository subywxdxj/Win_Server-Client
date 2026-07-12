#include "Recv.hpp"

bool SendFF(SOCKET& Socket, HCRYPTPROV& hCryptProv, const char keyMaster[DEFAULT_KEY], std::string Path);

bool SendFile(SOCKET& Socket, HCRYPTPROV& hCryptProv, std::string FileName, const char keyMaster[DEFAULT_KEY]);
bool SendFolder(SOCKET& Socket, HCRYPTPROV& hCryptProv, const char keyMaster[DEFAULT_KEY], std::string Path);