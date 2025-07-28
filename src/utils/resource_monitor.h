#pragma once
#include <unistd.h>

// CPU usage (%) of current process
double get_cpu_usage_self();

// Memory usage (bytes) of current process
size_t get_memory_usage_self();

// CPU usage (%) of a given PID (e.g., redis PID)
double get_cpu_usage_by_pid(pid_t pid);

// Memory usage (bytes) of a given PID
size_t get_memory_usage_by_pid(pid_t pid);
