//
// Created by artem on 26.12.2023.
//

#ifndef TESTTASK_WEIGHTEDROUNDROBIN_H
#define TESTTASK_WEIGHTEDROUNDROBIN_H

#include <vector>
#include <string>

class WeightedRoundRobin {
public:
    WeightedRoundRobin(const std::vector<std::pair<std::string, int>>& nodes_weights);

    std::string GetNextNode();

private:
    int gcdWeights();
    std::pair<std::string, int> maxWeight();

    const std::vector<std::pair<std::string, int>>& nodes_weights_;
    int totalServers_;
    int currentIndex_;
    int currentWeight_;
};

#endif //TESTTASK_WEIGHTEDROUNDROBIN_H
