#include <sys/socket.h>    
#include <arpa/inet.h>     
#include <unistd.h>        
#include <iostream>        
#include <string>          
#include <pthread.h>       
#include <cstring>         

int Connection; 


std::string xorEncryptDecrypt(const std::string& data, char key) {
    std::string result = data; 
    for (size_t i = 0; i < data.size(); ++i) { 
        result[i] ^= key; 
    }
    return result; 
}


void* ReceiveMessages(void* arg) {
    char msg[256]; 
    char key = 'K'; 
    while (true) { 
        ssize_t bytes_received = recv(Connection, msg, sizeof(msg) - 1, 0); 
        if (bytes_received > 0) {
            msg[bytes_received] = '\0'; 
            std::string encryptedMsg(msg); 
            std::string decryptedMsg = xorEncryptDecrypt(encryptedMsg, key); 
            std::cout << "Spy: " << decryptedMsg << std::endl; 
        } else if (bytes_received == 0) {
            std::cerr << "Connection closed by server." << std::endl;
            break;
        } else {
            std::cerr << "Error receiving data." << std::endl;
            break;
        }
    }
    return nullptr;
}

int main(int argc, char* argv[]) {
    struct sockaddr_in addr; 
    memset(&addr, 0, sizeof(addr)); 

    addr.sin_addr.s_addr = inet_addr("127.0.0.1"); 
    addr.sin_port = htons(1111); 
    addr.sin_family = AF_INET; 
    Connection = socket(AF_INET, SOCK_STREAM, 0); 
    if (Connection < 0) {
        std::cerr << "Failed to create socket!" << std::endl;
        return 1;
    }

    if (connect(Connection, (struct sockaddr*)&addr, sizeof(addr)) < 0) { 
        std::cerr << "Failed to connect to the server!" << std::endl;
        close(Connection);
        return 1;
    }

    std::cout << "Connected to the server!" << std::endl;

    pthread_t thread_id; 
    if (pthread_create(&thread_id, NULL, ReceiveMessages, NULL) != 0) { 
        std::cerr << "Failed to create thread!" << std::endl;
        close(Connection);
        return 1;
    }

    char msg[256]; 
    char key = 'K'; 

    while (true) { 
        std::cin.getline(msg, sizeof(msg)); 
        if (std::cin.eof()) break; 

        std::string originalMsg(msg); 
        std::string encryptedMsg = xorEncryptDecrypt(originalMsg, key); 
        if (send(Connection, encryptedMsg.c_str(), encryptedMsg.size() + 1, 0) < 0) { 
            std::cerr << "Error sending data." << std::endl;
            break;
        }
    }

    pthread_cancel(thread_id); 
    pthread_join(thread_id, NULL); 
    close(Connection); 
    return 0;
}