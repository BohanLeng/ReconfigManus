#include <iostream>
#include <fstream>
#include <nlohmann/json.hpp>
#include "MESServer.h"

using json = nlohmann::json;
#define DEFAULT_CONFIG_FILE "mes_server_cfg.json"
#define TEST_GRAPH_FILE "graph_model_example1.json"
#define TEST_STATION_CONFIG_FILE "station_process_cfg_example1.json"
#define PRODUCTS_FILE "products.json"

int main(int argc, char* argv[]) {
    const char* cfg_file = DEFAULT_CONFIG_FILE;
    uint16_t bind_port;
    uint8_t product_type;

    // TODO path to these configs could be included into `mes_server_cfg.json`
    // Load MES server config (bind port and product type)
    json j_cfg;
    try {
        std::ifstream f_cfg(cfg_file);
        j_cfg = json::parse(f_cfg);
        j_cfg["mes_service"]["bind_port"].get_to(bind_port);
        j_cfg["mes_service"]["product_type"].get_to(product_type);
    } catch (const std::exception& e) {
        std::cout << "Configuration file parsing failed (" << cfg_file << "):\n" << e.what();
        return 1;
    }

    // Load graph model JSON
    json j_graph;
    try {
        std::ifstream f_graph(TEST_GRAPH_FILE);
        j_graph = json::parse(f_graph);
    } catch (const std::exception& e) {
        std::cout << "Graph model file parsing failed (" << TEST_GRAPH_FILE << "):\n" << e.what();
        return 1;
    }

    // Load station process configuration JSON
    json j_station_cfg;
    try {
        std::ifstream f_station(TEST_STATION_CONFIG_FILE);
        j_station_cfg = json::parse(f_station);
    } catch (const std::exception& e) {
        std::cout << "Station config file parsing failed (" << TEST_STATION_CONFIG_FILE << "):\n" << e.what();
        return 1;
    }

    // 4) Load products JSON
    json j_products;
    try {
        std::ifstream f_products(PRODUCTS_FILE);
        j_products = json::parse(f_products);
    } catch (const std::exception& e) {
        std::cout << "Products file parsing failed (" << PRODUCTS_FILE << "):\n" << e.what();
        return 1;
    }

    // Construct server (managers may be wired elsewhere; here we only ensure four JSON objects are loaded)
    auto server = std::make_unique<MESServer>(bind_port, j_graph, j_station_cfg, j_products, product_type);
    server->CreateOrderBatch(10);

    std::cout << "MES Server started at port: " << bind_port << "\n";
    server->Start();
    server->Run();
}