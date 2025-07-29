#pragma once
#include <cstdint>
#include <vector>
#include <string>
#include <cryptopp/ripemd.h>
#include <cryptopp/hex.h>
#include <cryptopp/filters.h>
#include <aerospike/as_record.h>
#include <aerospike/as_bin.h>

#pragma pack(push, 0)
struct Details {
    std::string serial_no;
    uint16_t total_chapter;
    uint16_t total_page;
};
#pragma pack(pop)

#pragma pack(push, 0)
struct Materials {
    std::string cover;
    std::string content;
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
    CryptoPP::byte primary_key[CryptoPP::RIPEMD160::DIGESTSIZE];
    std::string key;
    std::string author;
    std::string title;
    uint16_t published_year;
    Details details;
    Materials materials;
    std::vector<Review> reviews;
    
    // Helper methods for Aerospike serialization
    void toAerospikeRecord(as_record* rec) const;
    void fromAerospikeRecord(const as_record* rec);
    std::string getDigestString() const;
};
#pragma pack(pop)
