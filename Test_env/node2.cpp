#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#include <cstdio>
#include <iostream>

#define PORT     8083
#define MAXLINE 1024

int main(){

    int sockfd;
    char buffer[MAXLINE];
    struct sockaddr_in servaddr, cliaddr;
    // Creating socket file descriptor
    if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&servaddr, 0, sizeof(servaddr));
    memset(&cliaddr, 0, sizeof(cliaddr));

    // Filling server information
    servaddr.sin_family    = AF_INET; // IPv4
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(PORT);
    fcntl(sockfd, F_SETFL, O_NONBLOCK);
    if ( bind(sockfd, (const struct sockaddr *)&servaddr,
              sizeof(servaddr)) < 0 )
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    int k = 0;
    int data_per_sec = 0;
    struct timeval startTime, currentTime;
    gettimeofday(&startTime, NULL);
    while(true) {
        socklen_t len;
        int n;

        len = sizeof(cliaddr);  //len is value/result
        fd_set readSet;
        FD_ZERO(&readSet);
        FD_SET(sockfd, &readSet);
        struct timeval timeout;
        timeout.tv_sec = 0;
        timeout.tv_usec = 0;
        if (select(sockfd + 1, &readSet, nullptr, nullptr, &timeout) > 0) {
            if (FD_ISSET(sockfd, &readSet)) {
                n = recvfrom(sockfd, (char *) buffer, MAXLINE,
                             0, (struct sockaddr *) &cliaddr,
                             &len);
                if (n == -1) {
                    std::cout << "RECEIVE ERROR" << std::endl;
                    perror("receive failed");
                    exit(EXIT_FAILURE);
                }
                buffer[n] = '\0';
                ++k;
                data_per_sec++;
            }
        }
        gettimeofday(&currentTime, NULL);

        double elapsedTime = (currentTime.tv_sec - startTime.tv_sec) + (currentTime.tv_usec - startTime.tv_usec) / 1000000.0;
        if(elapsedTime >= 1.0) {
            if(data_per_sec > 0) {
                std::cout << "K= " << k << " , data_per_sec= " << data_per_sec << std::endl;
            }
            gettimeofday(&startTime, NULL);
            data_per_sec = 0;
        }

    }
    return 0;
}
