#include "aerospike_connection.h"
#include <iostream>
#include <cstdlib>

AerospikeConnection::AerospikeConnection(const std::string& host, int port, 
                                       const std::string& ns, const std::string& set)
    : namespace_name(ns), set_name(set), connected(false) {
    
    // Initialize the configuration
    as_config_init(&config);
    
    // Set the host and port
    as_config_add_host(&config, host.c_str(), port);
    
    // Initialize the client
    aerospike_init(&as, &config);
}

AerospikeConnection::~AerospikeConnection() {
    if (connected) {
        disconnect();
    }
    aerospike_destroy(&as);
}

bool AerospikeConnection::connect() {
    as_error err;
    
    // Connect to the cluster
    if (aerospike_connect(&as, &err) != AEROSPIKE_OK) {
        std::cerr << "Failed to connect to Aerospike: " << err.message << std::endl;
        return false;
    }
    
    connected = true;
    std::cout << "Connected to Aerospike successfully!" << std::endl;
    return true;
}

void AerospikeConnection::disconnect() {
    if (connected) {
        as_error err;
        aerospike_close(&as, &err);
        connected = false;
        std::cout << "Disconnected from Aerospike." << std::endl;
    }
}
