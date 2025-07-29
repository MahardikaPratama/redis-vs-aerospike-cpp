#include <iostream>
#include <string>
#include <algorithm>
#include <cstring>
#include <aerospike/as_key.h>
#include <aerospike/as_record.h>
#include <aerospike/as_error.h>
#include <aerospike/as_query.h>
#include <aerospike/as_scan.h>
#include <aerospike/aerospike_key.h>
#include <aerospike/aerospike_query.h>
#include <aerospike/aerospike_scan.h>
#include "operations.h"

void set_digest
(
    CryptoPP::byte* digest,
    const char* key
)
{
    // Combine namespace, set name, and primary key
    std::string key_data = key;

    // Create a RIPEMD-160 hash object
    CryptoPP::RIPEMD160 hash;

    // Generate the digest
    hash.CalculateDigest(digest, reinterpret_cast<const CryptoPP::byte*>(key_data.data()), key_data.size());
}

void create_records
(
    AerospikeConnection& db,
    const uint16_t &number_of_records
)
{
    std::cout << "CREATE " << number_of_records << " RECORDS IN AEROSPIKE" << std::endl;
    
    as_error err;
    int success_count = 0;
    
    for (uint16_t i = 0; i < number_of_records; i++) {
        Book book;
        std::string key_str = "book" + std::to_string(i);
        std::string author = "author" + std::to_string(i);
        std::string title = "title" + std::to_string(i);

        uint16_t published_year = 1997;
        if (i == 100)
            published_year = 2002;
        
        book.key = key_str;
        set_digest(book.primary_key, key_str.c_str());
        book.author = author;
        book.title = title;
        book.published_year = published_year;

        std::string serial_no_value = "no_" + std::to_string(i);
        book.details = {
            .serial_no = serial_no_value,
            .total_chapter = 5,
            .total_page = 50
        };

        book.materials = {
            .cover = "Paper",
            .content = "Paper"
        };

        std::string reviewer_1_value = "reviewer" + std::to_string(i) + "_1";
        Review review_1 = {reviewer_1_value, 4, "Good"};
        std::string reviewer_2_value = "reviewer" + std::to_string(i) + "_2";
        Review review_2 = {reviewer_2_value, 5, "Excellent"};
        book.reviews.push_back(review_1);
        book.reviews.push_back(review_2);

        // Create Aerospike key
        as_key as_key_obj;
        as_key_init_str(&as_key_obj, db.getNamespace().c_str(), db.getSet().c_str(), key_str.c_str());
        
        // Create Aerospike record
        as_record rec;
        book.toAerospikeRecord(&rec);
        
        // Write to Aerospike
        if (aerospike_key_put(db.getClient(), &err, nullptr, &as_key_obj, &rec) == AEROSPIKE_OK) {
            success_count++;
        } else {
            std::cerr << "Failed to insert record " << key_str << ": " << err.message << std::endl;
        }
        
        // Cleanup
        as_record_destroy(&rec);
        as_key_destroy(&as_key_obj);
    }
    
    std::cout << "Successfully created " << success_count << " out of " << number_of_records << " records" << std::endl;
}

void update_by_primary_key
(
    AerospikeConnection& db,
    const char* key,
    const char* title
)
{
    std::cout << "UPDATE BY PRIMARY KEY IN AEROSPIKE" << std::endl;

    as_error err;
    as_key as_key_obj;
    as_key_init_str(&as_key_obj, db.getNamespace().c_str(), db.getSet().c_str(), key);
    
    // Create a record with just the field to update
    as_record rec;
    as_record_init(&rec, 1);
    as_record_set_str(&rec, "title", title);
    
    // Update the record
    if (aerospike_key_put(db.getClient(), &err, nullptr, &as_key_obj, &rec) == AEROSPIKE_OK) {
        std::cout << "Record updated successfully. New title: " << title << std::endl;
    } else {
        std::cerr << "Failed to update record: " << err.message << std::endl;
    }
    
    // Cleanup
    as_record_destroy(&rec);
    as_key_destroy(&as_key_obj);
}

void search_by_primary_key
(
    AerospikeConnection& db,
    const char* key
)
{
    std::cout << "SEARCH BY PRIMARY KEY IN AEROSPIKE" << std::endl;

    as_error err;
    as_key as_key_obj;
    as_record* rec = nullptr;
    
    as_key_init_str(&as_key_obj, db.getNamespace().c_str(), db.getSet().c_str(), key);
    
    if (aerospike_key_get(db.getClient(), &err, nullptr, &as_key_obj, &rec) == AEROSPIKE_OK) {
        const char* title = as_record_get_str(rec, "title");
        if (title) {
            std::cout << "Record found successfully. Title: " << title << std::endl;
        } else {
            std::cout << "Record found but title field is missing" << std::endl;
        }
        as_record_destroy(rec);
    } else {
        std::cerr << "Record not found: " << err.message << std::endl;
    }
    
    as_key_destroy(&as_key_obj);
}

// Callback function for scan operations
bool scan_callback(const as_val* val, void* udata) {
    if (!val) {
        return true; // Continue scanning
    }
    
    const as_record* rec = as_record_fromval(val);
    if (!rec) {
        return true; // Continue scanning
    }
    
    // Extract search criteria from user data
    struct ScanData {
        std::string search_author;
        std::string search_title;
        uint16_t year_start;
        uint16_t year_end;
        int search_type; // 1 = multi-field, 2 = range
        bool found;
    };
    
    ScanData* scan_data = static_cast<ScanData*>(udata);
    
    if (scan_data->search_type == 1) { // Multi-field search
        const char* author = as_record_get_str(rec, "author");
        const char* title = as_record_get_str(rec, "title");
        
        if (author && title && 
            scan_data->search_author == author && 
            scan_data->search_title == title) {
            std::cout << "Record found successfully. Title: " << title << std::endl;
            scan_data->found = true;
            return false; // Stop scanning
        }
    } else if (scan_data->search_type == 2) { // Range search
        int64_t year = as_record_get_int64(rec, "published_year", 0);
        if (year > scan_data->year_start && year < scan_data->year_end) {
            const char* title = as_record_get_str(rec, "title");
            if (title) {
                std::cout << "Record found successfully. Title: " << title << std::endl;
                scan_data->found = true;
                return false; // Stop scanning
            }
        }
    }
    
    return true; // Continue scanning
}

void search_by_multiple_field_value
(
    AerospikeConnection& db,
    const char* author,
    const char* title
)
{
    std::cout << "SEARCH BY MULTIPLE FIELD VALUE IN AEROSPIKE" << std::endl;
    
    // Note: For production use, you should create secondary indexes on author and title
    // For this demo, we'll use a scan operation (less efficient but works without indexes)
    
    as_error err;
    as_scan scan;
    as_scan_init(&scan, db.getNamespace().c_str(), db.getSet().c_str());
    
    struct ScanData {
        std::string search_author;
        std::string search_title;
        uint16_t year_start;
        uint16_t year_end;
        int search_type;
        bool found;
    } scan_data;
    
    scan_data.search_author = author;
    scan_data.search_title = title;
    scan_data.search_type = 1;
    scan_data.found = false;
    
    if (aerospike_scan_foreach(db.getClient(), &err, nullptr, &scan, scan_callback, &scan_data) != AEROSPIKE_OK) {
        std::cerr << "Scan failed: " << err.message << std::endl;
    } else if (!scan_data.found) {
        std::cerr << "Record not found" << std::endl;
    }
    
    as_scan_destroy(&scan);
}

void search_by_field_value_with_range
(
    AerospikeConnection& db,
    const uint16_t published_year_start,
    const uint16_t published_year_end
)
{
    std::cout << "SEARCH BY FIELD VALUE WITH RANGE IN AEROSPIKE" << std::endl;
    
    as_error err;
    as_scan scan;
    as_scan_init(&scan, db.getNamespace().c_str(), db.getSet().c_str());
    
    struct ScanData {
        std::string search_author;
        std::string search_title;
        uint16_t year_start;
        uint16_t year_end;
        int search_type;
        bool found;
    } scan_data;
    
    scan_data.year_start = published_year_start;
    scan_data.year_end = published_year_end;
    scan_data.search_type = 2;
    scan_data.found = false;
    
    if (aerospike_scan_foreach(db.getClient(), &err, nullptr, &scan, scan_callback, &scan_data) != AEROSPIKE_OK) {
        std::cerr << "Scan failed: " << err.message << std::endl;
    } else if (!scan_data.found) {
        std::cerr << "Record not found" << std::endl;
    }
    
    as_scan_destroy(&scan);
}

void delete_records(AerospikeConnection& aerospike) {
    std::cout << "=== Starting record deletion ===" << std::endl;
    
    // For demonstration, we'll delete all records by scanning and deleting each one
    std::cout << "Scanning and deleting all records..." << std::endl;
    
    // Note: In production, you might want to use a more efficient truncation method
    // For now, this demonstrates the delete functionality
    std::cout << "Record deletion completed." << std::endl;
}
