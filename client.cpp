#pragma comment(lib, "ws2_32.lib")
#include <winsock2.h>
#include <iostream>
#include <string>
#pragma warning(disable: 4996)

SOCKET Connection;


std::string xorEncryptDecrypt(const std::string& data, char key) {
    std::string result = data;
    for (size_t i = 0; i < data.size(); ++i) {
        result[i] ^= key;
    }
    return result;
}

void ReceiveMessages() {
    char msg[256];
    char key = 'G'; 
    while (true) {
        if (recv(Connection, msg, sizeof(msg), NULL) > 0) {
            std::string encryptedMsg(msg);
            std::string decryptedMsg = xorEncryptDecrypt(encryptedMsg, key); 
            std::cout << "Spy: " << decryptedMsg << std::endl;
        }
    }
}

int main(int argc, char* argv[]) {
    
    WSAData wsaData;
    WORD DLLVersion = MAKEWORD(2, 1);
    if (WSAStartup(DLLVersion, &wsaData) != 0) {
        std::cout << "WSAStartup failed!" << std::endl;
        return 1;
    }


    SOCKADDR_IN addr;
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    addr.sin_port = htons(1111);
    addr.sin_family = AF_INET;

    
    Connection = socket(AF_INET, SOCK_STREAM, NULL);
    if (connect(Connection, (SOCKADDR*)&addr, sizeof(addr)) != 0) {
        std::cout << "Failed to connect to the server!" << std::endl;
        return 1;
    }
    std::cout << "Connected to the server!" << std::endl;

   
    CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)ReceiveMessages, NULL, NULL, NULL);

 
    char msg[256];
    char key = 'K'; 
    while (true) {
        std::cin.getline(msg, sizeof(msg));
        std::string originalMsg(msg);
        std::string encryptedMsg = xorEncryptDecrypt(originalMsg, key); 
        send(Connection, encryptedMsg.c_str(), encryptedMsg.size() + 1, NULL);
    }

    closesocket(Connection);
    WSACleanup();
    return 0;
}