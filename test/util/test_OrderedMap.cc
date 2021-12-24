
#include "dasi/util/Test.h"

#include "dasi/util/OrderedMap.h"


CASE("Construct an ordered map and add things to it") {

    dasi::util::OrderedMap<std::string, std::string> om;
    EXPECT(om.empty());

    std::vector<std::pair<std::string, std::string>> expected{
            {"zzzz", "zvalue"},
            {"yyyy", "yvalue"},
            {"xxxx", "xvalue"}
    };

    for (const auto& v : expected) {
        om.insert(v);
    }

    EXPECT(om.size() == expected.size());
    for (const auto& v : expected) {
        EXPECT(om[v.first] == v.second);
    }

    int i = 0;
    for (const auto& kv : om) {
        EXPECT(kv.first == expected[i].first);
        EXPECT(kv.second == expected[i++].second);
    }
}

CASE("Construct an ordered map with an initialiser list") {

    dasi::util::OrderedMap<std::string, std::string> om {
        {"zzzz", "zvalue"},
        {"yyyy", "yvalue"},
        {"xxxx", "xvalue"}
    };

    std::vector<std::pair<std::string, std::string>> expected{
        {"zzzz", "zvalue"},
        {"yyyy", "yvalue"},
        {"xxxx", "xvalue"}
    };

    // Check values are accessible
    EXPECT(om.size() == expected.size());
    for (const auto& v : expected) {
        EXPECT(om[v.first] == v.second);
    }

    // Check values can be iterated
    int i = 0;
    for (const auto& kv : om) {
        EXPECT(kv.first == expected[i].first);
        EXPECT(kv.second == expected[i++].second);
    }
}

CASE("We can iterate with mutable values") {

    dasi::util::OrderedMap<std::string, std::string> om {
        {"zzzz", "zvalue"},
        {"yyyy", "yvalue"},
        {"xxxx", "xvalue"}
    };

    EXPECT(om.size() == 3);

    std::vector<std::string> updates = { "aaaa", "bbbb", "cccc"};

    int i = 0;
    for (auto& v : om) {
        v.second = updates[i++];
    }

    EXPECT(om["zzzz"] == "aaaa");
    EXPECT(om["yyyy"] == "bbbb");
    EXPECT(om["xxxx"] == "cccc");

    om["yyyy"] = "another-value";
    EXPECT(om["yyyy"] == "another-value");

    // And we can create new values

    EXPECT(om.find("pppp") == om.end());
    EXPECT(om.find("pppp") == om.end());
    om["pppp"];
    EXPECT(om.find("pppp") != om.end());
    EXPECT(om.find("pppp")->second.empty());
    om["pppp"] = "new value";
    EXPECT(om.find("pppp")->second == "new value");
}

CASE("Check that we can search for values as well") {

    dasi::util::OrderedMap<std::string, std::string> om {
            {"zzzz", "zvalue"},
            {"yyyy", "yvalue"},
            {"xxxx", "xvalue"}
    };

    auto it = om.find("yyyy");
    EXPECT(it != om.end());
    EXPECT(it->first == "yyyy");
    EXPECT(it->second == "yvalue");

    EXPECT(om.find("badkey") == om.end());
}

CASE("Iterators obtained via 'find' are not iterable") {

    // n.b. iteration order is determined by keys_ not the values_ dict.
    //      But lookups using 'find' use the values_ dict. Converting iterators between vector<>
    //      and map<> is not straightforward --> don't implement unless we really need it

    dasi::util::OrderedMap<std::string, std::string> om {
        {"zzzz", "zvalue"},
        {"yyyy", "yvalue"},
        {"xxxx", "xvalue"}
    };

    EXPECT(om.size() == 3);
    auto it = om.find("yyyy");
    EXPECT(it != om.end());
    EXPECT(it->second == "yvalue");
    EXPECT_THROWS_AS(++it, dasi::util::NotImplemented);
}

CASE("ordered map is printable") {

    dasi::util::OrderedMap<std::string, std::string> om {
        {"zzzz", "zvalue"},
        {"yyyy", "yvalue"},
        {"xxxx", "xvalue"}
    };

    EXPECT(om.size() == 3);
    std::ostringstream ss;
    ss << om;
    EXPECT(ss.str() == "{zzzz:zvalue, yyyy:yvalue, xxxx:xvalue}");
}

CASE("string_view as values") {

    dasi::util::OrderedMap<std::string, std::string_view> om {
        {"abcd", "efgh"},
        {"ijkl", "mnap"}
    };

    EXPECT(om.size() == 2);
    EXPECT(om["abcd"] == "efgh");
    EXPECT(om["ijkl"] == "mnap");

    om["ijkl"] = "newvalue";
    EXPECT(om["ijkl"] == "newvalue");
}

CASE("insert_or_assign") {
    dasi::util::OrderedMap<std::string, std::string> om;
    EXPECT(om.empty());

    std::vector<std::pair<std::string, std::string>> expected {
        {"zzzz", "zvalue"},
        {"yyyy", "yvalue"},
        {"xxxx", "xvalue"}
    };


    for (const auto& v : expected) {
        auto r = om.insert_or_assign(v.first, v.second);
        EXPECT(r.second);
        EXPECT(r.first->first == v.first);
        EXPECT(r.first->second == v.second);
    }

    EXPECT(om.size() == expected.size());
    for (const auto& v : expected) {
        EXPECT(om[v.first] == v.second);
    }

    int i = 0;
    for (const auto& kv : om) {
        EXPECT(kv.first == expected[i].first);
        EXPECT(kv.second == expected[i++].second);
    }

    std::vector<std::pair<std::string, std::string>> expected2 {
        {"zzzz", "zvalue"},
        {"yyyy", "newval"},
        {"xxxx", "xvalue"}
    };

    auto r = om.insert_or_assign("yyyy", "newval");
    EXPECT(!r.second);
    EXPECT(r.first->first == "yyyy");
    EXPECT(r.first->second == "newval");

    EXPECT(om.size() == expected2.size());
    for (const auto& v : expected2) {
        EXPECT(om[v.first] == v.second);
    }

    i = 0;
    for (const auto& kv : om) {
        EXPECT(kv.first == expected2[i].first);
        EXPECT(kv.second == expected2[i++].second);
    }
}

CASE("test emplace") {

    dasi::util::OrderedMap<std::string, std::string> om;
    EXPECT(om.empty());

    std::vector<std::pair<std::string, std::string>> expected{
        {"zzzz", "zvalue"},
        {"yyyy", "yvalue"},
        {"xxxx", "xvalue"}
    };

    for (const auto& v : expected) {
        om.emplace(std::make_pair(v.first, v.second));
    }

    EXPECT(om.size() == expected.size());
    for (const auto& v : expected) {
        EXPECT(om[v.first] == v.second);
    }

    int i = 0;
    for (const auto& kv : om) {
        EXPECT(kv.first == expected[i].first);
        EXPECT(kv.second == expected[i++].second);
    }
}

int main(int argc, char** argv) {
    return ::dasi::util::run_tests();
}