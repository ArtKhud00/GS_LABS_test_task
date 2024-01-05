//
// Created by artem on 25.12.2023.
//

#include <cstring>
#include <algorithm>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>
#include <poll.h>

#include "LoadBalancer.h"

LoadBalancer::LoadBalancer(const Config &config)
    : udp_socket_(CreateUDPSocket(config.udp_port))
    , nodes_with_weights_(config.node_addresses_with_weights)
    , max_datagrams_per_second_(config.max_datagrams_per_second)
    , weighted_rr_(nodes_with_weights_){
}

int LoadBalancer::CreateUDPSocket(int port) {
    int udp_socket = socket(AF_INET, SOCK_DGRAM, 0);
    if (udp_socket == -1) {
        std::cerr << "Error creating UDP socket" << std::endl;
        exit(EXIT_FAILURE);
    }
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(port);
    fcntl(udp_socket, F_SETFL, O_NONBLOCK);
    if (bind(udp_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        std::cerr << "Error binding UDP socket" << std::endl;
        close(udp_socket);
        exit(EXIT_FAILURE);
    }
    return udp_socket;
}

void LoadBalancer::ReceiveDatagrams() {
    int request_per_sec = 0;
    int total_datagrams = 0;
    std::chrono::time_point<std::chrono::steady_clock> start_time = std::chrono::steady_clock::now();
    std::chrono::time_point<std::chrono::steady_clock> current_time;
    while (true) {
        char buffer[1024];  // Размер буфера для принятых данных
        struct sockaddr_in client_addr;
        socklen_t client_addr_len = sizeof(client_addr);

        struct pollfd fds[1];
        fds[0].fd = udp_socket_;
        fds[0].events = POLLIN;

        if(request_per_sec < max_datagrams_per_second_) {
            // Check if there are any data to receive
            int result = poll(fds,1,0);
            if(result > 0) {
                if(fds[0].revents && POLL_IN) {
                    ssize_t bytesRead = recvfrom(udp_socket_, buffer, sizeof(buffer), 0,
                                                 (struct sockaddr *) &client_addr, &client_addr_len);
                    if (bytesRead == -1) {
                        std::cerr << "Error receiving datagram" << std::endl;
                    }
                    else {
                        std::string receivedDatagram(buffer, bytesRead);
                        request_per_sec++;
                        HandleDatagram(receivedDatagram);
                        total_datagrams++;
                        current_time = std::chrono::steady_clock::now();
                        auto elapsed_seconds = std::chrono::duration_cast<std::chrono::seconds>(current_time - start_time).count();
                        if(elapsed_seconds >= 1) {
                            // If a second has passed, reset the request_per_sec and update the start_time
                            std::cout<<"SECOND SPENT"<<std::endl;
                            request_per_sec = 0;
                            start_time = std::chrono::steady_clock::now();
                        }
                    }
                }
            }
        }
        else {
            current_time = std::chrono::steady_clock::now();
            auto elapsed_seconds = std::chrono::duration_cast<std::chrono::seconds>(current_time - start_time).count();
            if (elapsed_seconds >= 1) {
                // If a second has passed, reset the request_per_sec and update the start_time
                std::cout<<"SECOND SPENT, AMOUNT MAX, req_per_sec = "<<request_per_sec<<" ,total_datagrams = " << total_datagrams << std::endl;
                request_per_sec = 0;
                start_time = std::chrono::steady_clock::now();
            }
        }
    }
}

void LoadBalancer::HandleDatagram(const std::string& datagram){
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

void LoadBalancer::Run() {
    ReceiveDatagrams();
}

std::pair<std::string, int> LoadBalancer::ParseAddressPort(const std::string& address_port) {
    auto iter = std::find(address_port.begin(), address_port.end(), ':');
    auto pos = std::distance(address_port.begin(), iter);
    std::string ip_address = address_port.substr(0, pos);
    int port = std::stoi(address_port.substr(pos+1));
    return std::make_pair(ip_address, port);
}
