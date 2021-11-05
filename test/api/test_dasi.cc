
#include "dasi/util/Test.h"

#include "dasi/api/Dasi.h"

constexpr const char* TEST_CONFIG = R"(
schema:
 - [key1, key2, key3,
     [key1a, key2a, key3,
        [key1b, key2b, key3b],
        [key1B, key2B, key3B]],

     [key1A, key2A, key3,
        [key1b, key2b, key3b]],
     [Key1A, Key2A, Key3A,
        [key1b, key2b, key3b]]
   ]
 - [Key1, Key2, Key3,
     [key1a, key2a, key3,
        [key1b, key2b, key3b],
        [key1B, key2B, key3B]],

     [key1A, key2A, key3,
        [key1b, key2b, key3b]],
     [Key1A, Key2A, Key3A,
        [key1b, key2b, key3b]]
])";


CASE("Dasi simple archive") {
    dasi::Dasi dasi(TEST_CONFIG);

    dasi::Key key {
        {"key1", "value1"},
        {"key2", "value2"},
        {"key3", "value3"},
        {"key1a", "value1a"},
        {"key2a", "value2a"},
        {"key1b", "value1b"},
        {"key2b", "value2b"},
        {"key3b", "value3b"},
    };

    char test_data[] = "TESTING TESTING";
    dasi.archive(key, test_data, sizeof(test_data)-1);
}

CASE("Dasi archive with invalid key") {
    // Key that doesn't match the schema
    EXPECT(false);
}


int main(int argc, char** argv) {
    return ::dasi::run_tests();
}