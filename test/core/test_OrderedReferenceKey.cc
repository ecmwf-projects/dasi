
#include "dasi/util/Test.h"

#include "dasi/core/OrderedReferenceKey.h"


using namespace std::string_literals;

CASE("Construct from empty key") {

    dasi::core::OrderedReferenceKey k;
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

    // Test expected order

    std::vector<std::pair<std::string, std::string>> expected {
        {"key1", "value1"},
        {"key2", "value2"},
    };

    EXPECT(k.size() == expected.size());
    int i = 0;
    for (const auto& kv : k) {
        EXPECT(kv.first == expected[i].first);
        EXPECT(kv.second == expected[i++].second);
    }
}

CASE("Construct from initialiser list") {

    dasi::core::OrderedReferenceKey k {
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

    // Test expected order

    std::vector<std::pair<std::string, std::string>> expected {
        {"key1", "value1"},
        {"key2", "value2"},
        {"key3", "value3"},
    };

    EXPECT(k.size() == expected.size());
    int i = 0;
    for (const auto& kv : k) {
        EXPECT(kv.first == expected[i].first);
        EXPECT(kv.second == expected[i++].second);
    }
}

CASE("Modify existing key") {

    dasi::core::OrderedReferenceKey k {
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

    // Test expected order

    std::vector<std::pair<std::string, std::string>> expected {
        {"key1", "VALUE1a"},
        {"key2", "value2"},
        {"key3", "VALUE3b"},
    };

    EXPECT(k.size() == expected.size());
    int i = 0;
    for (const auto& kv : k) {
        EXPECT(kv.first == expected[i].first);
        EXPECT(kv.second == expected[i++].second);
    }
}

CASE("Test that clear() works") {

    dasi::core::OrderedReferenceKey k {
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

    // Test expected order

    std::vector<std::pair<std::string, std::string>> expected {
        {"key4", "value4"},
        {"key2", "value2"},
        {"key5", "value5"},
    };

    EXPECT(k.size() == expected.size());
    int i = 0;
    for (const auto& kv : k) {
        EXPECT(kv.first == expected[i].first);
        EXPECT(kv.second == expected[i++].second);
    }
}

int main(int argc, char** argv) {
    return ::dasi::util::run_tests();
}