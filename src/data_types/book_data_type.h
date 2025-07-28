#pragma once
#include <cstdint>
#include <string> 
#include <vector>

#pragma pack(push, 0)
struct Details {
    std::string serial_no;
    uint16_t total_chapter;
    uint16_t total_page;
};
#pragma pack(pop)

#pragma pack(push, 0)
struct Materials {
    std::string cover; // Changed from const char*
    std::string content; // Changed from const char*
};
#pragma pack(pop)

#pragma pack(push, 0)
struct Review {
    std::string reviewer;
    uint8_t rate;
    std::string comment;
};
#pragma pack(pop)

#pragma pack(push, 0)
struct Book {
    std::string key; // The 'key' field will serve as the Redis key.
    std::string author;
    std::string title;
    uint16_t published_year;
    Details details;
    Materials materials;
    std::vector<Review> reviews; 
};
#pragma pack(pop)

namespace nlohmann {
    template <typename T>
    struct adl_serializer;
}