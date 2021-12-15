#include "dasi/util/Test.h"

#include "dasi/api/Query.h"

#include <sstream>
#include <string>

using namespace std::string_literals;

CASE("Construct from empty request") {

    dasi::api::Query r;
    EXPECT(!r.has("key1"));
    EXPECT(!r.has("key2"));

    {
        std::ostringstream ss;
        ss << r;
        EXPECT(ss.str() == "{}");
    }

    r.set("key1", {"value1a", "value1b"});
    EXPECT(r.has("key1"));
    EXPECT(!r.has("key2"));
    EXPECT(!r.has("key3"));

    r.set("key2", {"value2"});
    EXPECT(r.has("key1"));
    EXPECT(r.has("key2"));
    EXPECT(!r.has("key3"));

    r.set("key3", {});
    EXPECT(r.has("key1"));
    EXPECT(r.has("key2"));
    EXPECT(r.has("key3"));

    {
        std::ostringstream ss;
        ss << r;
        EXPECT(ss.str() == "{key1:[value1a, value1b], key2:[value2], key3:[]}");
    }
}

CASE("Construct from initialiser list") {

    dasi::api::Query r {
            {"key1", {}},
            {"key2", {"value2"}},
            {"key3", {"value3a", "value3b"}},
    };

    EXPECT(r.has("key1"));
    EXPECT(r.has("key2"));
    EXPECT(r.has("key3"));
    EXPECT(!r.has("other"));

    std::ostringstream ss;
    ss << r;
    EXPECT(ss.str() == "{key1:[], key2:[value2], key3:[value3a, value3b]}");
}

CASE("Modify existing key") {

    dasi::api::Query r {
            {"key1", {"value1"}},
            {"key2", {"value2"}},
            {"key3", {"value3"}},
    };

    {
        std::ostringstream ss;
        ss << r;
        EXPECT(ss.str() == "{key1:[value1], key2:[value2], key3:[value3]}");
    }

    r.set("key1", {});
    r.set("key3", {"VALUE3a", "VALUE3b"});

    std::cout << "R: " << r << std::endl;
    {
        std::ostringstream ss;
        ss << r;
        EXPECT(ss.str() == "{key1:[], key2:[value2], key3:[VALUE3a, VALUE3b]}");
    }
}

int main(int argc, char** argv) {
    return ::dasi::util::run_tests();
}
