#include "book_data_type.h"
#include <sstream>
#include <iomanip>
#include <aerospike/as_string.h>
#include <aerospike/as_integer.h>

void Book::toAerospikeRecord(as_record* rec) const {
    as_record_init(rec, 10); // Initialize with expected number of bins
    
    // Basic fields
    as_record_set_str(rec, "key", key.c_str());
    as_record_set_str(rec, "author", author.c_str());
    as_record_set_str(rec, "title", title.c_str());
    as_record_set_int64(rec, "published_year", published_year);
    
    // Details
    as_record_set_str(rec, "serial_no", details.serial_no.c_str());
    as_record_set_int64(rec, "total_chapter", details.total_chapter);
    as_record_set_int64(rec, "total_page", details.total_page);
    
    // Materials
    as_record_set_str(rec, "cover_material", materials.cover.c_str());
    as_record_set_str(rec, "content_material", materials.content.c_str());
    
    // Store digest as hex string
    as_record_set_str(rec, "digest", getDigestString().c_str());
    
    // For simplicity, store reviews as JSON-like string
    // In production, you might want to use Aerospike's list/map types
    std::stringstream reviews_str;
    reviews_str << "[";
    for (size_t i = 0; i < reviews.size(); ++i) {
        if (i > 0) reviews_str << ",";
        reviews_str << "{\"reviewer\":\"" << reviews[i].reviewer 
                   << "\",\"rate\":" << static_cast<int>(reviews[i].rate)
                   << ",\"comment\":\"" << reviews[i].comment << "\"}";
    }
    reviews_str << "]";
    as_record_set_str(rec, "reviews", reviews_str.str().c_str());
}

void Book::fromAerospikeRecord(const as_record* rec) {
    // Extract basic fields
    if (as_record_get_str(rec, "key")) {
        key = as_record_get_str(rec, "key");
    }
    if (as_record_get_str(rec, "author")) {
        author = as_record_get_str(rec, "author");
    }
    if (as_record_get_str(rec, "title")) {
        title = as_record_get_str(rec, "title");
    }
    if (as_record_get_int64(rec, "published_year", 0)) {
        published_year = static_cast<uint16_t>(as_record_get_int64(rec, "published_year", 0));
    }
    
    // Extract details
    if (as_record_get_str(rec, "serial_no")) {
        details.serial_no = as_record_get_str(rec, "serial_no");
    }
    if (as_record_get_int64(rec, "total_chapter", 0)) {
        details.total_chapter = static_cast<uint16_t>(as_record_get_int64(rec, "total_chapter", 0));
    }
    if (as_record_get_int64(rec, "total_page", 0)) {
        details.total_page = static_cast<uint16_t>(as_record_get_int64(rec, "total_page", 0));
    }
    
    // Extract materials
    if (as_record_get_str(rec, "cover_material")) {
        materials.cover = as_record_get_str(rec, "cover_material");
    }
    if (as_record_get_str(rec, "content_material")) {
        materials.content = as_record_get_str(rec, "content_material");
    }
    
    // Note: For full implementation, you'd parse the reviews JSON string back to vector
    // This is simplified for demonstration
}

std::string Book::getDigestString() const {
    std::stringstream ss;
    ss << std::hex << std::setfill('0');
    for (int i = 0; i < CryptoPP::RIPEMD160::DIGESTSIZE; ++i) {
        ss << std::setw(2) << static_cast<unsigned>(primary_key[i]);
    }
    return ss.str();
}
