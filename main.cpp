#include "LoadBalancer.h"
#include "configuration_parse.h"

int main() {
    Config config_struct;
    config_struct = ParseConfigurationFile("config_bal.json");
    LoadBalancer load_balancer(config_struct);
    load_balancer.run();
    return 0;
}
