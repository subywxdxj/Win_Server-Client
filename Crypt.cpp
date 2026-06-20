#include "Crypt.h"

//         [(1      + 255 )      + 256      ] = 512
//packet - [(header + data)^keyR + keyR^keyM]


char* Crypt(const char* data, const char* key, int size)
{
    //xor data with key and key << 256
    char* res = new char[size];
    res[0] = data[0] ^ key[size - 1] ^ key[0];
    for (int i = 1; i < size; i++)
    {
        res[i] = data[i] ^ key[i] ^ key[i - 1];
    }

    return res;
}

const char* CryptPacket(const char* data, const char keyRand[DEFAULT_KEY], const char keyMaster[DEFAULT_KEY], int size)
{
    char* packetCrypt = new char[size*2];//data+key
    std::memcpy(packetCrypt, Crypt(data, keyRand, size), size);//encrypt data with 1 time key
    std::memcpy(packetCrypt + size, Crypt(keyRand, keyMaster, size), size);//encrypt 1 time key with master key

    return packetCrypt;
}

int sendCrypt(SOCKET& s, const char* data, const char keyRand[DEFAULT_KEY], const char keyMaster[DEFAULT_KEY], int size, int flags)//adding Crypt() to send()
{
    return send(s, CryptPacket(data, keyRand, keyMaster, size), size*2, flags);
}

char* recvCrypt(SOCKET& s,const char* keyMaster, int size, int flags, int& result)//recv() and decypt
{
    char* buf = new char[size*2];
    result = recv(s, buf, size*2, flags) / 2;//half of the packet is the key

    const char* keyRand = Crypt(buf + size, keyMaster, size);

    return Crypt(buf, keyRand, size);
}

//DEBUG
void DebugChar(char* buf, int size)
{
    std::cout << "\nDBG-INF " << size << "\n";
    for (int i = 0; i < size; i++)
    {
        std::cout << (int)buf[i] << ", ";
    }
    std::cout << "\n";
}