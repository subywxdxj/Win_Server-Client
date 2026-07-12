#include "File.hpp"

bool RecvFF(SOCKET& Socket, const char keyMaster[DEFAULT_KEY]);

bool RecvFile(SOCKET& Socket, const char keyMaster[DEFAULT_KEY]);
bool RecvFolder(SOCKET& Socket, const char keyMaster[DEFAULT_KEY]);

std::string RecvFolderPath(SOCKET& Socket, const char keyMaster[DEFAULT_KEY]);