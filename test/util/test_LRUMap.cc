
#include "dasi/util/Test.h"

#include "dasi/util/LRUMap.h"
#include "dasi/util/ReverseAdapter.h"

#include <algorithm>
#include <chrono>
#include <thread>

using namespace std::chrono_literals;

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

    EXPECT(lm.size() == expected.size());
    for (const auto& v : expected) {
        EXPECT(lm[v.first] == v.second);
    }

    int i = 0;
    for (const auto& kv : lm) {
        EXPECT(kv.first == expected[i].first);
        EXPECT(kv.second == expected[i++].second);
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

    EXPECT(lm.size() == lm.capacity());
    for (int i = 0; i < expected.size(); ++i) {
        if (i < lm.capacity()) {
            EXPECT(lm[expected[i].first] == expected[i].second);
        } else {
            EXPECT_THROWS_AS(lm[expected[i].first], dasi::util::KeyError);
        }
    }

    int i = 0;
    for (const auto& kv : lm) {
        EXPECT(kv.first == expected[i].first);
        EXPECT(kv.second == expected[i++].second);
    }

    EXPECT(lm.capacity() == 5);
}


int main(int argc, char** argv) {
    return ::dasi::util::run_tests();
}