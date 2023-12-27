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
#include "WeightedRoundRobin.h"

struct Config {
    int udpPort;
    std::vector<std::pair<std::string, int>> node_addresses_with_weights;
    int maxDatagramsPerSecond;
};

class LoadBalancer {
public:
    LoadBalancer(const Config& config);
    void run();

private:
    int createUDPSocket(int port);
    void handleDatagram(const std::string& datagram);
    void receiveDatagrams();
    std::pair<std::string, int> ParseAddressPort(const std::string& );

    int udp_socket_;
    std::vector<std::pair<std::string,int>> nodes_with_weights_;
    int max_datagrams_per_second_;
    WeightedRoundRobin weighted_rr_;
    std::chrono::steady_clock::time_point lastResetTime_;
};

#endif //TESTTASK_LOADBALANCER_H
