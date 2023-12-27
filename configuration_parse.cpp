//
// Created by artem on 26.12.2023.
//

#include "configuration_parse.h"
#include <fstream>
#include "json.h"

Config ParseConfigurationFile(const std::string& filename){
    Config config_struct;
    std::ifstream config_file(filename);
    if(config_file.good() && !config_file.eof()){
        json::Document doc = json::Load(config_file);
        json::Node root = doc.GetRoot();
        json::Dict params = root.AsDict();
        std::vector<std::string> nodes;
        std::vector<int> nodes_weights;
        if(params.count("src_port") > 0){
            config_struct.udpPort = params.at("src_port").AsInt();
        }
        if(params.count("nodes_adresses") > 0){
            json::Array nodes_array = params.at("nodes_adresses").AsArray();
            for(const auto& node : nodes_array){
                nodes.push_back(node.AsString());
            }
        }
        if(params.count("nodes_weights") > 0){
            json::Array nodes_weights_val = params.at("nodes_weights").AsArray();
            for(const auto& weight : nodes_weights_val){
                nodes_weights.push_back(weight.AsInt());
            }
        }
        if(params.count("max_datagrams_per_second")>0){
            config_struct.maxDatagramsPerSecond = params.at("max_datagrams_per_second").AsInt();
        }
        if(!nodes.empty()){
            if(nodes_weights.empty() || nodes_weights.size() != nodes.size()){
                for(const std::string node : nodes){
                    config_struct.node_addresses_with_weights.emplace_back(std::make_pair(node, 1));
                }
            }
            else{
                for(int i =0; i < nodes.size(); ++i){
                    config_struct.node_addresses_with_weights.emplace_back(std::make_pair(nodes[i],nodes_weights[i]));
                }
            }
        }
    }
    return config_struct;
}