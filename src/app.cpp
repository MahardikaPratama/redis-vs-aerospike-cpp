#include <chrono>
#include <iostream>
#include <vector>
#include <cstdlib>
#include "operations/operations.h"
#include "database/aerospike_connection.h"
#include "utils/resource_monitor.h"

int64_t get_time_in_us() {
    auto now = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(now.time_since_epoch());

    return duration.count();
}

int main() {
    // Get Aerospike connection parameters from environment or use defaults
    std::string aerospike_host = std::getenv("AEROSPIKE_HOST") ? std::getenv("AEROSPIKE_HOST") : "localhost";
    int aerospike_port = std::getenv("AEROSPIKE_PORT") ? std::atoi(std::getenv("AEROSPIKE_PORT")) : 43120;
    std::string aerospike_namespace = std::getenv("AEROSPIKE_NAMESPACE") ? std::getenv("AEROSPIKE_NAMESPACE") : "test";
    std::string aerospike_set = std::getenv("AEROSPIKE_SET") ? std::getenv("AEROSPIKE_SET") : "books";
    
    std::cout << "Connecting to Aerospike at " << aerospike_host << ":" << aerospike_port << std::endl;
    std::cout << "Namespace: " << aerospike_namespace << ", Set: " << aerospike_set << std::endl;
    
    // Initialize Aerospike connection
    AerospikeConnection db(aerospike_host, aerospike_port, aerospike_namespace, aerospike_set);
    
    if (!db.connect()) {
        std::cerr << "Failed to connect to Aerospike. Exiting." << std::endl;
        return 1;
    }
    
    // Create vector for timing comparison (not used for storage anymore)
    const uint16_t number_of_records = 10000; 
    
    std::cout << "=================================================================================" << std::endl;
    auto start = get_time_in_us();
    create_records(db, number_of_records);
    std::cout << "Time: " + std::to_string(get_time_in_us() - start) << " us" << std::endl;

    // Update a record by primary key
    std::cout << "=================================================================================" << std::endl;
    start = get_time_in_us();
    update_by_primary_key(db, "book100", "title10o");
    std::cout << "Time: " + std::to_string(get_time_in_us() - start) << " us" << std::endl;

    // Read the records by primary key
    std::cout << "=================================================================================" << std::endl;
    start = get_time_in_us();
    search_by_primary_key(db, "book100");
    std::cout << "Time: " + std::to_string(get_time_in_us() - start) << " us" << std::endl;

    // Read the records by multiple bin value
    std::cout << "=================================================================================" << std::endl;
    start = get_time_in_us();
    search_by_multiple_field_value(db, "author100", "title10o"); // Updated to match the changed title
    std::cout << "Time: " + std::to_string(get_time_in_us() - start) << " us" << std::endl;

    // Read the records by bin value with range
    std::cout << "=================================================================================" << std::endl;
    start = get_time_in_us();
    search_by_field_value_with_range(db, 2000, 2005);
    std::cout << "Time: " + std::to_string(get_time_in_us() - start) << " us" << std::endl;

    // Delete the records
    // std::cout << "=================================================================================" << std::endl;
    // start = get_time_in_us();
    // delete_records(db);
    // std::cout << "Time: " + std::to_string(get_time_in_us() - start) << " us" << std::endl;

        // =====================================================================
    // Resource Usage Monitoring
    // =====================================================================
    std::cout << "=================================================================================" << std::endl;

    double app_cpu = get_cpu_usage_self();
    size_t app_mem = get_memory_usage_self();

    std::cout << "Application resources usage" << std::endl;
    std::cout << "CPU: " << app_cpu << "%, Memory: " << app_mem / (1024.0 * 1024.0) << " MB" << std::endl;

    pid_t aerospike_pid = 0;
    FILE* cmd = popen("pgrep asd", "r");
    if (cmd) {
        fscanf(cmd, "%d", &aerospike_pid);
        pclose(cmd);
    }

    if (aerospike_pid > 0) {
        double as_cpu = get_cpu_usage_by_pid(aerospike_pid);
        size_t as_mem = get_memory_usage_by_pid(aerospike_pid);

        std::cout << "Aerospike server resources usage" << std::endl;
        std::cout << "CPU: " << as_cpu << "%, Memory: " << as_mem / (1024.0 * 1024.0) << " MB" << std::endl;
    } else {
        std::cerr << "Aerospike process not found (make sure 'asd' is running)" << std::endl;
    }

    return 0;
}
