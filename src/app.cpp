#include <chrono>
#include <iostream>
#include <string>
#include <sw/redis++/redis++.h> 
#include "operations/operations.h"
#include "utils/resource_monitor.h"

int64_t get_time_in_us() {
    auto now = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(now.time_since_epoch());
    return duration.count();
}

int main() {
    sw::redis::Redis redis("tcp://127.0.0.1:6379");
    
    // Check connection
    try {
        redis.ping();
        std::cout << "Success connect to Redis" << std::endl;
    } catch (const sw::redis::Error &err) {
        std::cerr << "Failed to connect to Redis: " << err.what() << std::endl;
        return 1;
    }

    const uint16_t number_of_records = 10000;

    std::cout << "=================================================================================" << std::endl;
    auto start = get_time_in_us();
    create_records_redis(redis, number_of_records);
    std::cout << "Time: " + std::to_string(get_time_in_us() - start) << " us" << std::endl;

    std::cout << "=================================================================================" << std::endl;
    start = get_time_in_us();
    update_by_primary_key_redis(redis, "book100", "title10o");
    std::cout << "Time: " + std::to_string(get_time_in_us() - start) << " us" << std::endl;

    std::cout << "=================================================================================" << std::endl;
    start = get_time_in_us();
    search_by_primary_key_redis(redis, "book100");
    std::cout << "Time: " + std::to_string(get_time_in_us() - start) << " us" << std::endl;

    std::cout << "=================================================================================" << std::endl;
    start = get_time_in_us();
    search_by_multiple_field_value_redis(redis, "author100", "title100", number_of_records);
    std::cout << "Time: " + std::to_string(get_time_in_us() - start) << " us" << std::endl;

    std::cout << "=================================================================================" << std::endl;
    start = get_time_in_us();
    search_by_field_value_with_range_redis(redis, 2000, 2005, number_of_records);
    std::cout << "Time: " + std::to_string(get_time_in_us() - start) << " us" << std::endl;

    std::cout << "=================================================================================" << std::endl;
    start = get_time_in_us();
    delete_records_redis(redis, number_of_records);
    std::cout << "Time: " + std::to_string(get_time_in_us() - start) << " us" << std::endl;

    // =====================================================================
    // Resource Usage Monitoring
    // =====================================================================
    std::cout << "=================================================================================" << std::endl;

    double app_cpu = get_cpu_usage_self();
    size_t app_mem = get_memory_usage_self();

    std::cout << "Application resources usage" << std::endl;
    std::cout << "CPU: " << app_cpu << "%, Memory: " << app_mem / (1024.0 * 1024.0) << " MB" << std::endl;

    pid_t redis_pid = 0;
    FILE* cmd = popen("pgrep -x redis-server", "r");
    if (cmd) {
    if (fscanf(cmd, "%d", &redis_pid) != 1) { 
        redis_pid = 0; // Reset redis_pid if fscanf fails
    }
    pclose(cmd);
}

    if (redis_pid > 0) {
        double as_cpu = get_cpu_usage_by_pid(redis_pid);
        size_t as_mem = get_memory_usage_by_pid(redis_pid);

        std::cout << "redis server resources usage" << std::endl;
        std::cout << "CPU: " << as_cpu << "%, Memory: " << as_mem / (1024.0 * 1024.0) << " MB" << std::endl;
    } else {
        std::cerr << "redis process not found (make sure 'asd' is running)" << std::endl;
    }

    return 0;
}
