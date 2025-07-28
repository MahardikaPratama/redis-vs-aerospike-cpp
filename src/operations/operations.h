#pragma once
#include <vector>
#include <string>
#include <sw/redis++/redis++.h> 
#include "../data_types/book_data_type.h"
#include <nlohmann/json.hpp>

void to_json(nlohmann::json& j, const Details& d);
void from_json(const nlohmann::json& j, Details& d);

void to_json(nlohmann::json& j, const Materials& m);
void from_json(const nlohmann::json& j, Materials& m);

void to_json(nlohmann::json& j, const Review& r);
void from_json(const nlohmann::json& j, Review& r);

void to_json(nlohmann::json& j, const Book& b);
void from_json(const nlohmann::json& j, Book& b);


void create_records_redis
(
    sw::redis::Redis& redis,
    const uint16_t &number_of_records
);

void update_by_primary_key_redis
(
    sw::redis::Redis& redis,
    const char* key,
    const char* title
);

std::optional<Book> search_by_primary_key_redis
(
    sw::redis::Redis& redis,
    const char* key
);

std::optional<Book> search_by_multiple_field_value_redis
(
    sw::redis::Redis& redis,
    const char* author,
    const char* title,
    const uint16_t &number_of_records 
);

std::optional<Book> search_by_field_value_with_range_redis
(
    sw::redis::Redis& redis,
    const uint16_t published_year_start,
    const uint16_t published_year_end,
    const uint16_t &number_of_records 
);

void delete_records_redis
(
    sw::redis::Redis& redis,
    const uint16_t &number_of_records
);

// Function to get Redis info (memory, keyspace)
std::string get_redis_info(sw::redis::Redis& redis, const std::string& section);