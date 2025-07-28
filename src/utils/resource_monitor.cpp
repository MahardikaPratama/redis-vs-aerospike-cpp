#include "resource_monitor.h"
#include <fstream>
#include <sstream>
#include <string>
#include <thread>
#include <chrono>
#include <iostream>

long get_total_cpu_time() {
    std::ifstream file("/proc/stat");
    std::string line;
    std::getline(file, line); 
    std::istringstream ss(line);
    std::string cpu_label;
    long user, nice, system, idle, iowait, irq, softirq;
    ss >> cpu_label >> user >> nice >> system >> idle >> iowait >> irq >> softirq;
    return user + nice + system + idle + iowait + irq + softirq;
}

long get_process_cpu_time(pid_t pid) {
    std::ifstream stat_file("/proc/" + std::to_string(pid) + "/stat");
    if (!stat_file) return 0;

    std::string token;
    for (int i = 1; i <= 13; ++i) stat_file >> token; 
    long utime = 0, stime = 0;
    stat_file >> utime >> stime;
    return utime + stime;
}

double get_cpu_usage_by_pid(pid_t pid) {
    long clk_tck = sysconf(_SC_CLK_TCK);
    long num_cpus = sysconf(_SC_NPROCESSORS_ONLN);

    long proc_time1 = get_process_cpu_time(pid);
    long total_time1 = get_total_cpu_time();

    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    long proc_time2 = get_process_cpu_time(pid);
    long total_time2 = get_total_cpu_time();

    long delta_proc = proc_time2 - proc_time1;
    long delta_total = total_time2 - total_time1;

    if (delta_total <= 0 || clk_tck == 0) return 0.0;

    double usage = (double)delta_proc / delta_total * 100.0 * num_cpus;
    return usage;
}

double get_cpu_usage_self() {
    return get_cpu_usage_by_pid(getpid());
}

size_t get_memory_usage_self() {
    std::ifstream status_file("/proc/self/status");
    std::string line;
    while (std::getline(status_file, line)) {
        if (line.substr(0, 6) == "VmRSS:") {
            std::istringstream iss(line);
            std::string key;
            size_t value_kb;
            std::string unit;
            iss >> key >> value_kb >> unit;
            return value_kb * 1024;
        }
    }
    return 0;
}

size_t get_memory_usage_by_pid(pid_t pid) {
    std::ifstream status_file("/proc/" + std::to_string(pid) + "/status");
    std::string line;
    while (std::getline(status_file, line)) {
        if (line.substr(0, 6) == "VmRSS:") {
            std::istringstream iss(line);
            std::string key;
            size_t value_kb;
            std::string unit;
            iss >> key >> value_kb >> unit;
            return value_kb * 1024;
        }
    }
    return 0;
}
