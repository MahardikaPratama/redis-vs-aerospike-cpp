#pragma once
#include <vector>
#include "../data_types/book_data_type.h"
#include "../database/aerospike_connection.h"

void create_records
(
    AerospikeConnection& db,
    const uint16_t &number_of_records
);
void update_by_primary_key
(
    AerospikeConnection& db,
    const char* key,
    const char* title
);
void search_by_primary_key
(
    AerospikeConnection& db,
    const char* key
);
void search_by_multiple_field_value
(
    AerospikeConnection& db,
    const char* author,
    const char* title
);
void search_by_field_value_with_range
(
    AerospikeConnection& db,
    const uint16_t published_year_start,
    const uint16_t published_year_end
);
void delete_records(AerospikeConnection& db);
