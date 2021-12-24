
#include "dasi/util/Test.h"

#include "dasi/util/LRUMap.h"
#include "dasi/util/ReverseAdapter.h"

#include <algorithm>
#include <chrono>
#include <thread>

using namespace std::chrono_literals;

template <typename T, typename S>
int checkExpected(const dasi::util::LRUMap<T, S>& lm,
                  const std::vector<std::pair<T, S>>& expected) {

//    std::cout << "CHECK: " << std::endl;
//    std::cout << lm << std::endl;
//    std::cout << expected << std::endl;
    EXPECT(lm.size() == expected.size());

    int i = 0;
    for (const auto& kv : lm) {
        EXPECT(kv.first == expected[i].first);
        EXPECT(kv.second == expected[i++].second);
    }

    return i;
}



CASE("Construct comprehensive tests for LRUMap (not written yet!!!)") {
    dasi::util::LRUMap<std::string, std::string> lm(5);
    EXPECT(lm.empty());

    std::vector<std::pair<std::string, std::string>> expected{
        {"zzzz", "zvalue"},
        {"yyyy", "yvalue"},
        {"xxxx", "xvalue"}
    };

    for (const auto& v : dasi::util::ReverseAdapter(expected)) {
        std::this_thread::sleep_for(10ms);
        lm.insert(v);
    }

    checkExpected(lm, expected);

    EXPECT(lm.size() == expected.size());
    for (const auto& v : expected) {
        EXPECT(lm[v.first] == v.second);
    }
}

CASE("LRUMap has a size, and things get pushed off it") {
    dasi::util::LRUMap<std::string, std::string> lm(5);
    EXPECT(lm.empty());

    std::vector<std::pair<std::string, std::string>> expected {
        {"zzzz", "zvalue"},
        {"yyyy", "yvalue"},
        {"xxxx", "xvalue"},
        {"wwww", "wvalue"},
        {"vvvv", "vvalue"},
        {"uuuu", "uvalue"},
        {"tttt", "tvalue"},
    };

    EXPECT(lm.capacity() == 5);

    for (const auto& v : dasi::util::ReverseAdapter(expected)) {
        std::this_thread::sleep_for(10ms);
        lm.insert(v);
    }

    int i = 0;
    for (const auto& kv : lm) {
        EXPECT(kv.first == expected[i].first);
        EXPECT(kv.second == expected[i++].second);
    }

    EXPECT(lm.size() == lm.capacity());
    for (int i = 0; i < expected.size(); ++i) {
        if (i < lm.capacity()) {
            EXPECT(lm[expected[i].first] == expected[i].second);
        } else {
            EXPECT_THROWS_AS(lm[expected[i].first], dasi::util::KeyError);
        }
    }

    EXPECT(lm.capacity() == 5);
}

CASE("Construct an LRU map with an initialiser list") {

    dasi::util::LRUMap<std::string, std::string> lm (5, {
        {"zzzz", "zvalue"},
        {"yyyy", "yvalue"},
        {"xxxx", "xvalue"}
    });

    std::vector<std::pair<std::string, std::string>> expected {
        {"zzzz", "zvalue"},
        {"yyyy", "yvalue"},
        {"xxxx", "xvalue"}
    };

    checkExpected(lm, expected);

    // Check values are accessible
    EXPECT(lm.size() == expected.size());
    for (const auto& v : expected) {
        EXPECT(lm[v.first] == v.second);
    }
}

CASE("Access via find() re-orders the data") {

    dasi::util::LRUMap<std::string, std::string> lm (5, {
        {"zzzz", "zvalue"},
        {"yyyy", "yvalue"},
        {"xxxx", "xvalue"},
    });

    std::vector<std::pair<std::string, std::string>> expected1 {
        {"zzzz", "zvalue"},
        {"yyyy", "yvalue"},
        {"xxxx", "xvalue"},
    };

    std::vector<std::pair<std::string, std::string>> expected2 {
        {"yyyy", "yvalue"},
        {"zzzz", "zvalue"},
        {"xxxx", "xvalue"},
    };

    std::vector<std::pair<std::string, std::string>> expected3 {
        {"xxxx", "xvalue"},
        {"yyyy", "yvalue"},
        {"zzzz", "zvalue"},
    };

    checkExpected(lm, expected1);

    // Accessing the first element does nothing

    {
        auto it = lm.find("zzzz");
        EXPECT(it->first == "zzzz");
        EXPECT(it->second == "zvalue");
    }

    checkExpected(lm, expected1);

    // Accessing the second element does a reshuffle

    {
        auto it = lm.find("yyyy");
        EXPECT(it->first == "yyyy");
        EXPECT(it->second == "yvalue");
    }

    checkExpected(lm, expected2);

    // Also works for the last element

    {
        auto it = lm.find("xxxx");
        EXPECT(it->first == "xxxx");
        EXPECT(it->second == "xvalue");
    }

    checkExpected(lm, expected3);
}

CASE("Access via operator[] reorders the data") {

    dasi::util::LRUMap<std::string, std::string> lm (5, {
        {"zzzz", "zvalue"},
        {"yyyy", "yvalue"},
        {"xxxx", "xvalue"},
    });

    std::vector<std::pair<std::string, std::string>> expected1 {
        {"zzzz", "zvalue"},
        {"yyyy", "yvalue"},
        {"xxxx", "xvalue"},
    };

    std::vector<std::pair<std::string, std::string>> expected2 {
        {"yyyy", "yvalue"},
        {"zzzz", "zvalue"},
        {"xxxx", "xvalue"},
    };

    std::vector<std::pair<std::string, std::string>> expected3 {
        {"xxxx", "xvalue"},
        {"yyyy", "yvalue"},
        {"zzzz", "zvalue"},
    };

    checkExpected(lm, expected1);

    // Accessing the first element does nothing

    EXPECT(lm["zzzz"] == "zvalue");
    checkExpected(lm, expected1);

    // Accessing the second element does a reshuffle

    EXPECT(lm["yyyy"] == "yvalue");
    checkExpected(lm, expected2);

    // Also works for the last element

    EXPECT(lm["xxxx"] == "xvalue");
    checkExpected(lm, expected3);
}

CASE("We can iterate with mutable values") {

    dasi::util::LRUMap<std::string, std::string> lm (5, {
        {"zzzz", "zvalue"},
        {"yyyy", "yvalue"},
        {"xxxx", "xvalue"}
    });

    EXPECT(lm.size() == 3);

    std::vector<std::string> updates = { "aaaa", "bbbb", "cccc"};

    int i = 0;
    for (auto& v : lm) {
        v.second = updates[i++];
    }

    EXPECT(lm["zzzz"] == "aaaa");
    EXPECT(lm["yyyy"] == "bbbb");
    EXPECT(lm["xxxx"] == "cccc");

    lm["yyyy"] = "another-value";
    EXPECT(lm["yyyy"] == "another-value");

    // And we cannot create new values

    EXPECT(lm.find("pppp") == lm.end());
    EXPECT(lm.find("pppp") == lm.end());
    EXPECT_THROWS_AS(lm["pppp"], dasi::util::KeyError);
}

CASE("Check that we can search for values as well") {

    dasi::util::LRUMap<std::string, std::string> lm(5, {
        {"zzzz", "zvalue"},
        {"yyyy", "yvalue"},
        {"xxxx", "xvalue"}
    });

    auto it = lm.find("yyyy");
    EXPECT(it != lm.end());
    EXPECT(it->first == "yyyy");
    EXPECT(it->second == "yvalue");

    EXPECT(lm.find("badkey") == lm.end());
}

CASE("LRU map is printable") {

    dasi::util::LRUMap<std::string, std::string> lm(5, {
        {"zzzz", "zvalue"},
        {"yyyy", "yvalue"},
        {"xxxx", "xvalue"}
    });

    EXPECT(lm.size() == 3);
    std::ostringstream ss;
    ss << lm;
    EXPECT(ss.str() == "{zzzz:zvalue, yyyy:yvalue, xxxx:xvalue}");
}

CASE("string_view as values") {

    dasi::util::LRUMap<std::string, std::string_view> lm (3, {
        {"abcd", "efgh"},
        {"ijkl", "mnap"}
    });

    EXPECT(lm.size() == 2);
    EXPECT(lm["abcd"] == "efgh");
    EXPECT(lm["ijkl"] == "mnap");

    lm["ijkl"] = "newvalue";
    EXPECT(lm["ijkl"] == "newvalue");
}

CASE("test emplace") {

    dasi::util::LRUMap<std::string, std::string> lm(5);
    EXPECT(lm.empty());

    std::vector<std::pair<std::string, std::string>> expected{
        {"zzzz", "zvalue"},
        {"yyyy", "yvalue"},
        {"xxxx", "xvalue"}
    };

    for (const auto& v: dasi::util::ReverseAdapter(expected)) {
        std::this_thread::sleep_for(10ms);
        std::pair<std::string, std::string> copy(v.first, v.second);
        lm.emplace(std::move(copy));
    }

    checkExpected(lm, expected);

    EXPECT(lm.size() == expected.size());
    for (const auto& v: expected) {
        EXPECT(lm[v.first] == v.second);
    }
}

int main(int argc, char** argv) {
    return ::dasi::util::run_tests();
}