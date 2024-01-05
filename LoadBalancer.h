//
// Created by artem on 25.12.2023.
//

#ifndef TESTTASK_LOADBALANCER_H
#define TESTTASK_LOADBALANCER_H

#include <iostream>
#include <chrono>
#include <fstream>
#include <sstream>
#include <vector>
#include <unistd.h>
#include <arpa/inet.h>
#include <csignal>
#include "WeightedRoundRobin.h"

struct Config {
    int udp_port;
    std::vector<std::pair<std::string, int>> node_addresses_with_weights;
    int max_datagrams_per_second;
};

class LoadBalancer {
public:
    LoadBalancer(const Config& config);
    void Run();

private:
    int CreateUDPSocket(int port);
    void HandleDatagram(const std::string& datagram);
    void ReceiveDatagrams();
    std::pair<std::string, int> ParseAddressPort(const std::string& address_port);

    int udp_socket_;
    std::vector<std::pair<std::string,int>> nodes_with_weights_;
    int max_datagrams_per_second_;
    WeightedRoundRobin weighted_rr_;
};

#endif //TESTTASK_LOADBALANCER_H
