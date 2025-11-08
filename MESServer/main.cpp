#include <iostream>
#include <fstream>
#include <nlohmann/json.hpp>
#include "MESServer.h"

using json = nlohmann::json;
#define DEFAULT_CONFIG_FILE "mes_server_cfg.json"
#define TEST_GRAPH_FILE "graph_model_example_big.json"

int main(int argc, char* argv[]) {
    const char* cfg_file = DEFAULT_CONFIG_FILE;
    unsigned int bind_port;
    if (argc > 1) {
        cfg_file = argv[1];
    }
    std::ifstream f(cfg_file);
    try {
        json j = json::parse(f);
        bind_port = j["mes_service"]["bind_port"];
    } catch (std::exception& e) {
        std::cout << "Configuration file parsing failed:\n"
                << e.what();
        return 1;
    }

    auto server = std::make_unique<MESServer>(bind_port);
    std::cout << "MES Server started at port: " << bind_port << "\n";
    server->Start();
    server->Run();
}