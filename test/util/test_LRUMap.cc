
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


int main(int argc, char** argv) {
    return ::dasi::util::run_tests();
}