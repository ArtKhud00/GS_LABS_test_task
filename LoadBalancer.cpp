//
// Created by artem on 25.12.2023.
//

#include <cstring>
#include <algorithm>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <thread>
#include "LoadBalancer.h"

LoadBalancer::LoadBalancer(const Config &config)
    : udp_socket_(createUDPSocket(config.udpPort))
    , nodes_with_weights_(config.node_addresses_with_weights)
    , max_datagrams_per_second_(config.maxDatagramsPerSecond)
    , weighted_rr_(nodes_with_weights_)
    {
}

int LoadBalancer::createUDPSocket(int port) {
    int udpSocket = socket(AF_INET, SOCK_DGRAM, 0);
    if (udpSocket == -1) {
        std::cerr << "Error creating UDP socket" << std::endl;
        exit(EXIT_FAILURE);
    }
    struct sockaddr_in serverAddr;
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serverAddr.sin_port = htons(port);

    if (bind(udpSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
        std::cerr << "Error binding UDP socket" << std::endl;
        close(udpSocket);
        exit(EXIT_FAILURE);
    }
    return udpSocket;
}

void LoadBalancer::receiveDatagrams() {
    int request_per_sec = 0;
    int k = 0;
    lastResetTime_ = std::chrono::steady_clock::now();
    while (true) {
        auto currentTime = std::chrono::steady_clock::now();
        char buffer[1024];  // Размер буфера для принятых данных
        struct sockaddr_in clientAddr;
        socklen_t clientAddrLen = sizeof(clientAddr);
        ssize_t bytesRead = recvfrom(udp_socket_, buffer, sizeof(buffer), 0,
                                     (struct sockaddr*)&clientAddr, &clientAddrLen);
        if (bytesRead == -1) {
            std::cerr << "Error receiving datagram" << std::endl;
        } else {
            std::string receivedDatagram(buffer, bytesRead);
            ++request_per_sec;
            if(request_per_sec < max_datagrams_per_second_) {
                handleDatagram(receivedDatagram);
                ++k;
            }
            auto elapsedSeconds = std::chrono::duration_cast<std::chrono::seconds>(currentTime - lastResetTime_).count();
            if(elapsedSeconds >=1){
                //std::cout << "request_per_sec = " << request_per_sec << std::endl;
                lastResetTime_ = currentTime;
                request_per_sec = 0;
            }
        }
    }
}

void LoadBalancer::handleDatagram(const std::string& datagram){
    std::string server_to_send = weighted_rr_.GetNextNode();
    struct sockaddr_in client_addr;
    memset(&client_addr, 0, sizeof(client_addr));
    client_addr.sin_family = AF_INET;
    std::pair<std::string, int> address_port = ParseAddressPort(server_to_send);
    client_addr.sin_addr.s_addr = 0;
    inet_pton(AF_INET, address_port.first.c_str(), &(client_addr.sin_addr));
    client_addr.sin_port = htons(address_port.second);
    const char* data = datagram.c_str();
    size_t data_length = datagram.length();
    sendto(udp_socket_, data, data_length,
           0, (const struct sockaddr *) &client_addr,
           sizeof(client_addr));
}

void LoadBalancer::run() {
    receiveDatagrams();
}

std::pair<std::string, int> LoadBalancer::ParseAddressPort(const std::string& address_port) {
    auto iter = std::find(address_port.begin(), address_port.end(), ':');
    auto pos = std::distance(address_port.begin(), iter);
    std::string ip_address = address_port.substr(0, pos);
    int port = std::stoi(address_port.substr(pos+1));
    return std::make_pair(ip_address, port);
}