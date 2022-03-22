
#include "dasi/util/Test.h"

#include "dasi/api/SplitKey.h"

#include <sstream>

CASE("Construct from empty key") {

    dasi::api::SplitKey k;
    EXPECT(!k[0].has("key1"));
    EXPECT(!k[1].has("key1"));
    EXPECT(!k[2].has("key1"));
    EXPECT(!k[0].has("key2"));
    EXPECT(!k[1].has("key2"));
    EXPECT(!k[2].has("key2"));
    EXPECT(!k[0].has("key3"));
    EXPECT(!k[1].has("key3"));
    EXPECT(!k[2].has("key3"));

    {
        std::ostringstream ss;
        ss << k;
        EXPECT(ss.str() == "[{}, {}, {}]");
    }

    k[0].set("key1", "value1");
    EXPECT(k[0].has("key1"));
    EXPECT(!k[1].has("key1"));
    EXPECT(!k[2].has("key1"));
    EXPECT(!k[0].has("key2"));
    EXPECT(!k[1].has("key2"));
    EXPECT(!k[2].has("key2"));
    EXPECT(!k[0].has("key3"));
    EXPECT(!k[1].has("key3"));
    EXPECT(!k[2].has("key3"));

    k[1].set("key2", "value2");
    EXPECT(k[0].has("key1"));
    EXPECT(k[1].has("key2"));
    EXPECT(!k[1].has("key1"));
    EXPECT(!k[2].has("key1"));
    EXPECT(!k[0].has("key2"));
    EXPECT(!k[2].has("key2"));
    EXPECT(!k[0].has("key3"));
    EXPECT(!k[1].has("key3"));
    EXPECT(!k[2].has("key3"));

    k[2].set("key3", "value3");
    EXPECT(k[0].has("key1"));
    EXPECT(k[1].has("key2"));
    EXPECT(k[2].has("key3"));
    EXPECT(!k[1].has("key1"));
    EXPECT(!k[2].has("key1"));
    EXPECT(!k[0].has("key2"));
    EXPECT(!k[2].has("key2"));
    EXPECT(!k[0].has("key3"));
    EXPECT(!k[1].has("key3"));

    {
        std::ostringstream ss;
        ss << k;
        EXPECT(ss.str() == "[{key1:value1}, {key2:value2}, {key3:value3}]");
    }
}

CASE("Construct from initialiser list") {

    dasi::api::SplitKey k {
        {
            {"key1", "value1"},
        },
        {
            {"key2", "value2"},
        },
        {
            {"key3", "value3"},
        },
    };
    EXPECT(k[0].has("key1"));
    EXPECT(k[1].has("key2"));
    EXPECT(k[2].has("key3"));
    EXPECT(!k[1].has("key1"));
    EXPECT(!k[2].has("key1"));
    EXPECT(!k[0].has("key2"));
    EXPECT(!k[2].has("key2"));
    EXPECT(!k[0].has("key3"));
    EXPECT(!k[1].has("key3"));


    {
        std::ostringstream ss;
        ss << k;
        EXPECT(ss.str() == "[{key1:value1}, {key2:value2}, {key3:value3}]");
    }
}

CASE("Construct from existing SplitKey") {

    dasi::api::SplitKey base {
        {
            {"key1", "value1"},
        },
        {
            {"key2", "value2"},
        },
        {
            {"key3", "value3"},
        },
    };

    dasi::api::SplitKey k(base);
    EXPECT(k[0].has("key1"));
    EXPECT(k[1].has("key2"));
    EXPECT(k[2].has("key3"));
    EXPECT(!k[1].has("key1"));
    EXPECT(!k[2].has("key1"));
    EXPECT(!k[0].has("key2"));
    EXPECT(!k[2].has("key2"));
    EXPECT(!k[0].has("key3"));
    EXPECT(!k[1].has("key3"));

    k[1].set("key2", "newvalue2");
    k[2].set("newkey", "value");

    {
        std::ostringstream ss;
        ss << base;
        EXPECT(ss.str() == "[{key1:value1}, {key2:value2}, {key3:value3}]");
    }

    {
        std::ostringstream ss;
        ss << k;
        EXPECT(ss.str() == "[{key1:value1}, {key2:newvalue2}, {key3:value3, newkey:value}]");
    }
}

#if 0
CASE("Test constructing SplitKey from SplitReferenceKey") {
    EXPECT(false);
}
#endif

CASE("Modify existing key") {

    dasi::api::SplitKey k {
        {
            {"key1", "value1"},
        },
        {
            {"key2", "value2"},
        },
        {
            {"key3", "value3"},
        },
    };

    {
        std::ostringstream ss;
        ss << k;
        EXPECT(ss.str() == "[{key1:value1}, {key2:value2}, {key3:value3}]");
    }

    k[0].set("key1", "othervalue1");
    k[2].set("key3", "newvalue3");

    {
        std::ostringstream ss;
        ss << k;
        EXPECT(ss.str() == "[{key1:othervalue1}, {key2:value2}, {key3:newvalue3}]");
    }

    k[1].set("key2", "changed2");

    {
        std::ostringstream ss;
        ss << k;
        EXPECT(ss.str() == "[{key1:othervalue1}, {key2:changed2}, {key3:newvalue3}]");
    }
}


int main(int argc, char** argv) {
    return ::dasi::util::run_tests();
}
