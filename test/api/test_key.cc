
#include "dasi/util/Test.h"

#include "dasi/api/Key.h"

#include <sstream>
#include <string>

using namespace std::string_literals;

CASE("Construct from empty key") {

    dasi::api::Key k;
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

    EXPECT(k.size() == 2);
}

CASE("Construct from initialiser list") {

    dasi::api::Key k {
        {"key1", "value1"},
        {"key2", "value2"},
        {"key3", "value3"},
    };

    EXPECT(k.has("key1"));
    EXPECT(k.has("key2"));
    EXPECT(k.has("key3"));
    EXPECT(!k.has("other"));

    EXPECT(k.size() == 3);

    std::ostringstream ss;
    ss << k;
    EXPECT(ss.str() == "{key1:value1, key2:value2, key3:value3}");
}

CASE("Modify existing key") {

    dasi::api::Key k {
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

    EXPECT(k.size() == 3);
}

CASE("Test that clear() works") {

    dasi::api::Key k {
        {"key1", "value1"},
        {"key2", "value2"},
        {"key3", "value3"},
    };

    EXPECT(k.size() == 3);
    k.clear();
    EXPECT(k.size() == 0);

    for (const auto& kv : k) {
        EXPECT(false);
    }

    k.set("key4", "value4");
    k.set("key2", "value2");
    k.set("key5", "value5");

    EXPECT(k.size() == 3);
}

int main(int argc, char** argv) {
    return ::dasi::util::run_tests();
}