#include <iostream>
#include <algorithm>
#include <optional>
#include <vector>
#include "operations.h"
#include <nlohmann/json.hpp> 

using json = nlohmann::json;

void to_json(json& j, const Details& d) {
    j = json{{"serial_no", d.serial_no}, {"total_chapter", d.total_chapter}, {"total_page", d.total_page}};
}

void from_json(const json& j, Details& d) {
    j.at("serial_no").get_to(d.serial_no);
    j.at("total_chapter").get_to(d.total_chapter);
    j.at("total_page").get_to(d.total_page);
}

void to_json(json& j, const Materials& m) {
    j = json{{"cover", m.cover}, {"content", m.content}};
}

void from_json(const json& j, Materials& m) {
    j.at("cover").get_to(m.cover);
    j.at("content").get_to(m.content);
}

void to_json(json& j, const Review& r) {
    j = json{{"reviewer", r.reviewer}, {"rate", r.rate}, {"comment", r.comment}};
}

void from_json(const json& j, Review& r) {
    j.at("reviewer").get_to(r.reviewer);
    j.at("rate").get_to(r.rate);
    j.at("comment").get_to(r.comment);
}

void to_json(json& j, const Book& b) {
    j = json{
        {"key", b.key},
        {"author", b.author},
        {"title", b.title},
        {"published_year", b.published_year},
        {"details", b.details},
        {"materials", b.materials},
        {"reviews", b.reviews}
    };
}

void from_json(const json& j, Book& b) {
    j.at("key").get_to(b.key);
    j.at("author").get_to(b.author);
    j.at("title").get_to(b.title);
    j.at("published_year").get_to(b.published_year);
    j.at("details").get_to(b.details);
    j.at("materials").get_to(b.materials);
    j.at("reviews").get_to(b.reviews);
}


void create_records_redis
(
    sw::redis::Redis& redis,
    const uint16_t &number_of_records
)
{
    std::cout << "CREATE 10,000 RECORDS" << std::endl;
    for (uint16_t i = 0; i < number_of_records; i++) {
        Book book;
        std::string key = "book" + std::to_string(i);
        std::string author = "author" + std::to_string(i);
        std::string title = "title" + std::to_string(i);

        uint16_t published_year = 1997;
        if (i == 100)
            published_year = 2002;
        
        book.key = key;
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

        json book_json = book;
        std::string book_str = book_json.dump();

        redis.set(book.key, book_str);
    }
}

void update_by_primary_key_redis
(
    sw::redis::Redis& redis,
    const char* key,
    const char* title
)
{
    std::cout << "UPDATE BY PRIMARY KEY" << std::endl;
    std::optional<std::string> book_str_opt = redis.get(key);

    if (book_str_opt) {
        Book book = json::parse(book_str_opt.value());
        book.title = title;
        json book_json = book;
        redis.set(key, book_json.dump());
        std::cout << "Record update successfully. Title value:" + std::string(book.title) << std::endl;
    } else {
        std::cerr << "Record not found" << std::endl;
    }
}

std::optional<Book> search_by_primary_key_redis
(
    sw::redis::Redis& redis,
    const char* key
)
{
    std::cout << "SEARCH BY PRIMARY KEY" << std::endl;
    std::optional<std::string> book_str_opt = redis.get(key);

    if (book_str_opt) {
        Book book = json::parse(book_str_opt.value());
        std::cout << "Record read successfully. Title value:" + std::string(book.title) << std::endl;
        return book;
    } else {
        std::cerr << "Record not found" << std::endl;
        return std::nullopt;
    }
}

std::optional<Book> search_by_multiple_field_value_redis
(
    sw::redis::Redis& redis,
    const char* author,
    const char* title,
    const uint16_t &number_of_records 
)
{
    std::cout << "SEARCH BY MULTIPLE FIELD VALUE" << std::endl;
    sw::redis::Cursor cursor = 0;
    std::vector<std::string> keys;
    std::string pattern = "book*"; 

    do {
        cursor = redis.scan(cursor, pattern, 1000, std::back_inserter(keys));
    } while (cursor != 0);

    for (const auto& key : keys) {
        std::optional<std::string> book_str_opt = redis.get(key);
        if (book_str_opt) {
            Book book = json::parse(book_str_opt.value());
            if (book.author == author && book.title == title) {
                std::cout << "Record read successfully. Title value:" + std::string(book.title) << std::endl;
                return book;
            }
        }
    }
    std::cerr << "Record not found" << std::endl;
    return std::nullopt;
}

std::optional<Book> search_by_field_value_with_range_redis
(
    sw::redis::Redis& redis,
    const uint16_t published_year_start,
    const uint16_t published_year_end,
    const uint16_t &number_of_records 
)
{
    std::cout << "SEARCH BY FIELD VALUE WITH RANGE" << std::endl;
    sw::redis::Cursor cursor = 0;
    std::vector<std::string> keys;
    std::string pattern = "book*"; 

    do {
        cursor = redis.scan(cursor, pattern, 1000, std::back_inserter(keys));
    } while (cursor != 0);

    for (const auto& key : keys) {
        std::optional<std::string> book_str_opt = redis.get(key);
        if (book_str_opt) {
            Book book = json::parse(book_str_opt.value());
            if (book.published_year > published_year_start && book.published_year < published_year_end) {
                std::cout << "Record read successfully. Title value:" + std::string(book.title) << std::endl;
                return book;
            }
        }
    }
    std::cerr << "Record not found" << std::endl;
    return std::nullopt;
}

void delete_records_redis(sw::redis::Redis& redis, const uint16_t &number_of_records) {
    std::cout << "DELETE RECORDS" << std::endl;
    std::vector<std::string> keys_to_delete;
    for (uint16_t i = 0; i < number_of_records; ++i) {
        keys_to_delete.push_back("book" + std::to_string(i));
    }
    if (!keys_to_delete.empty()) {
        redis.del(keys_to_delete.begin(), keys_to_delete.end());
    }
}

std::string get_redis_info(sw::redis::Redis& redis, const std::string& section) {
    return redis.info(section);
}