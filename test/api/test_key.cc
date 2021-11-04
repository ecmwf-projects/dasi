
#include "dasi/core/Test.h"

#include "dasi/api/Key.h"

#include <sstream>
#include <string>

using namespace std::string_literals;

CASE("Construct from empty key") {

    dasi::Key k;
    EXPECT(!k.has("key1"));
    EXPECT(!k.has("key2"));

    {
        std::ostringstream ss;
        ss << k;
        EXPECT(ss.str() == "{}");
    }

    k.set("key1", "value1");
    EXPECT(k.has("key1"));
    EXPECT(!k.has("key2"));

    k.set("key2", "value2");
    EXPECT(k.has("key1"));
    EXPECT(k.has("key2"));

    {
        std::ostringstream ss;
        ss << k;
        EXPECT(ss.str() == "{key1:value1, key2:value2}");
    }
}

CASE("Construct from initialiser list") {

    dasi::Key k {
        {"key1", "value1"},
        {"key2", "value2"},
        {"key3", "value3"},
    };

    EXPECT(k.has("key1"));
    EXPECT(k.has("key2"));
    EXPECT(k.has("key3"));
    EXPECT(!k.has("other"));

    std::ostringstream ss;
    ss << k;
    EXPECT(ss.str() == "{key1:value1, key2:value2, key3:value3}");
}

CASE("Modify existing key") {

    dasi::Key k {
        {"key1", "value1"},
        {"key2", "value2"},
        {"key3", "value3"},
    };

    {
        std::ostringstream ss;
        ss << k;
        EXPECT(ss.str() == "{key1:value1, key2:value2, key3:value3}");
    }

    k.set("key1", "VALUE1a");
    k.set("key3", "VALUE3b");

    {
        std::ostringstream ss;
        ss << k;
        EXPECT(ss.str() == "{key1:VALUE1a, key2:value2, key3:VALUE3b}");
    }
}

int main(int argc, char** argv) {
    return ::dasi::run_tests();
}