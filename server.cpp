#include <iostream>
#include <string>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

int Connections[2]; 
int Counter = 0;    


void* ExchangeMessages(void* arg) {
    char msg[256]; 
    while (true) { 
        
        if (recv(Connections[0], msg, sizeof(msg), 0) > 0) {
            std::cout << "Encrypted message from Client 1: " << msg << std::endl;
            send(Connections[1], msg, sizeof(msg), 0); 
        }
        
        if (recv(Connections[1], msg, sizeof(msg), 0) > 0) {
            std::cout << "Encrypted message from Client 2: " << msg << std::endl;
            send(Connections[0], msg, sizeof(msg), 0); 
        }
    }
    return nullptr;
}

int main(int argc, char* argv[]) {
    int sListen; 
    sockaddr_in addr; 
    int sizeofaddr = sizeof(addr); 

    
    sListen = socket(AF_INET, SOCK_STREAM, 0);
    if (sListen == -1) {
        std::cerr << "Socket creation failed!" << std::endl;
        return 1;
    }

    
    addr.sin_family = AF_INET; 
    addr.sin_addr.s_addr = inet_addr("127.0.0.1"); 
    addr.sin_port = htons(1111); 
    
    if (bind(sListen, (sockaddr*)&addr, sizeof(addr)) == -1) {
        std::cerr << "Binding failed!" << std::endl;
        close(sListen);
        return 1;
    }

   
    if (listen(sListen, 2) == -1) {
        std::cerr << "Listen failed!" << std::endl;
        close(sListen);
        return 1;
    }

    std::cout << "Server is waiting for connections..." << std::endl;

    
    for (int i = 0; i < 2; i++) {
        Connections[i] = accept(sListen, (sockaddr*)&addr, (socklen_t*)&sizeofaddr);
        if (Connections[i] == -1) { 
            std::cerr << "Client connection failed!" << std::endl;
            close(sListen);
            return 1;
        }
        std::cout << "Client " << i + 1 << " connected!" << std::endl; 
        Counter++; 
    }

    
    pthread_t thread;
    if (pthread_create(&thread, nullptr, ExchangeMessages, nullptr) != 0) {
        std::cerr << "Failed to create thread!" << std::endl;
        close(sListen);
        for (int i = 0; i < 2; i++) {
            close(Connections[i]);
        }
        return 1;
    }

    
    pthread_join(thread, nullptr);

    
    close(sListen);
    for (int i = 0; i < 2; i++) {
        close(Connections[i]);
    }

    return 0;
}
