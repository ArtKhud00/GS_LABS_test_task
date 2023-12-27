//
// Created by artem on 26.12.2023.
//

#include <numeric>
#include <algorithm>
#include "WeightedRoundRobin.h"

WeightedRoundRobin::WeightedRoundRobin(const std::vector<std::pair<std::string, int> > &nodes_weights)
    : nodes_weights_(nodes_weights)
    , totalServers_(nodes_weights.size())
    , currentIndex_(0)
    , currentWeight_(0) {}

int WeightedRoundRobin::gcdWeights() {
    int result = nodes_weights_[0].second;
    for (int i = 1; i < totalServers_; ++i) {
        result = std::gcd(result, nodes_weights_[i].second);
    }
    return result;
}

std::pair<std::string, int> WeightedRoundRobin::maxWeight() {
    auto maxElement = std::max_element(nodes_weights_.begin(), nodes_weights_.end(),
                                       [](const auto& lhs, const auto& rhs) {
                                           return lhs.second < rhs.second;
                                       }
    );
    return *maxElement;
}

std::string WeightedRoundRobin::GetNextNode() {
    while(true) {
        currentIndex_ = (currentIndex_ + 1) % totalServers_;
        if (currentIndex_ == 0) {
            currentWeight_ = currentWeight_ - gcdWeights();
            if (currentWeight_ <= 0) {
                currentWeight_ = maxWeight().second;
                if (currentWeight_ == 0) {
                    return ""; // All weights are zero, no available server
                }
            }
        }

        if (nodes_weights_[currentIndex_].second >= currentWeight_) {
            return nodes_weights_[currentIndex_].first;
        }
    }
}
