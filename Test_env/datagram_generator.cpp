#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <chrono>

#define PORT     8081
#define MAXLINE 1024

int main(){
    int sockfd;
    char buffer[MAXLINE];
    struct sockaddr_in     servaddr;

    if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(PORT);
    servaddr.sin_addr.s_addr = INADDR_ANY;

    int n;
    socklen_t len;

    auto startTime_ = std::chrono::steady_clock::now();
    for(int i = 0; i < 250000; ++i) {
        std::string str = "Hello" + std::to_string(i);
        const char* hello = str.c_str();
        sendto(sockfd, hello, strlen(hello),
               0, (const struct sockaddr *) &servaddr,
               sizeof(servaddr));
    }
    auto finishTime_ = std::chrono::steady_clock::now();
    auto elapsedSeconds = std::chrono::duration_cast<std::chrono::seconds>(finishTime_ - startTime_).count();
    std::cout << "Elapsed seconds:" << elapsedSeconds << std::endl;
}
