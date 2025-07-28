cmake_minimum_required(VERSION 3.10)
project(redis_performance_test CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include_directories(/usr/local/include)
link_directories(/usr/local/lib)

find_package(PkgConfig REQUIRED)
pkg_check_modules(REDISPP REQUIRED redis++)

include_directories(${REDISPP_INCLUDE_DIRS})
link_directories(${REDISPP_LIBRARY_DIRS})
add_definitions(${REDISPP_CFLAGS_OTHER})


add_subdirectory(external/json) 

include_directories(src/data_types)

add_executable(redis_test
    src/app.cpp
    src/operations/operations.cpp
    src/utils/resource_monitor.cpp
)

target_link_libraries(redis_test
    PRIVATE
    hiredis
    ${REDISPP_LIBRARIES}
    nlohmann_json::nlohmann_json 
)