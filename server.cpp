#pragma comment(lib, "ws2_32.lib")
#include <winsock2.h>
#include <iostream>
#include <string>
#pragma warning(disable: 4996)

SOCKET Connections[2]; 
int Counter = 0;       

void ExchangeMessages() {
    char msg[256];
    while (true) {
        
        if (recv(Connections[0], msg, sizeof(msg), NULL) > 0) {
            std::cout << "Encrypted message from Client 1: " << msg << std::endl;
            send(Connections[1], msg, sizeof(msg), NULL);
        }

        if (recv(Connections[1], msg, sizeof(msg), NULL) > 0) {
            std::cout << "Encrypted message from Client 2: " << msg << std::endl;
            send(Connections[0], msg, sizeof(msg), NULL); 
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
    int sizeofaddr = sizeof(addr);
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    addr.sin_port = htons(1111);
    addr.sin_family = AF_INET;


    SOCKET sListen = socket(AF_INET, SOCK_STREAM, NULL);
    bind(sListen, (SOCKADDR*)&addr, sizeof(addr));
    listen(sListen, 2); 
    std::cout << "Server is waiting for connections..." << std::endl;

   
    for (int i = 0; i < 2; i++) {
        Connections[i] = accept(sListen, (SOCKADDR*)&addr, &sizeofaddr);
        if (Connections[i] == INVALID_SOCKET) {
            std::cout << "Client connection failed!" << std::endl;
            return 1;
        }
        std::cout << "Client " << i + 1 << " connected!" << std::endl;
        Counter++;
    }

    ExchangeMessages();

    closesocket(sListen);
    for (int i = 0; i < 2; i++) {
        closesocket(Connections[i]);
    }
    WSACleanup();
    return 0;
}