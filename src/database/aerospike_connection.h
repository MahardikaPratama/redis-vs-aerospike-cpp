#pragma once
#include <aerospike/aerospike.h>
#include <aerospike/aerospike_key.h>
#include <aerospike/as_config.h>
#include <aerospike/as_error.h>
#include <aerospike/as_record.h>
#include <aerospike/as_status.h>
#include <aerospike/as_operations.h>
#include <aerospike/as_bin.h>
#include <aerospike/as_scan.h>
#include <aerospike/aerospike_scan.h>
#include <string>

class AerospikeConnection {
private:
    aerospike as;
    as_config config;
    std::string namespace_name;
    std::string set_name;
    bool connected;

public:
    AerospikeConnection(const std::string& host = "localhost", 
                       int port = 3001, 
                       const std::string& ns = "test", 
                       const std::string& set = "books");
    ~AerospikeConnection();
    
    bool connect();
    void disconnect();
    bool isConnected() const { return connected; }
    
    aerospike* getClient() { return &as; }
    const std::string& getNamespace() const { return namespace_name; }
    const std::string& getSet() const { return set_name; }
};
