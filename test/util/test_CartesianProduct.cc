
#include "dasi/util/Test.h"

#include "dasi/util/CartesianProduct.h"
#include "dasi/util/ContainerIostream.h"
#include "dasi/util/Exceptions.h"

#include <cstring>

using namespace std::string_literals;

//----------------------------------------------------------------------------------------------------------------------

CASE("Test output ordering") {
    std::vector<int> output(3);

    std::vector<int> v1 {1, 2, 3};
    std::vector<int> v2 {4, 5, 6};
    std::vector<int> v3 {7, 8, 9};

    dasi::util::CartesianProduct<std::vector<int>> cp;

    cp.append(v1, output[0]);
    cp.append(v2, output[2]);
    cp.append(v3, output[1]);

    std::vector<std::vector<int>> expected_output {
        {1, 7, 4}, {2, 7, 4}, {3, 7, 4},
        {1, 7, 5}, {2, 7, 5}, {3, 7, 5},
        {1, 7, 6}, {2, 7, 6}, {3, 7, 6},
        {1, 8, 4}, {2, 8, 4}, {3, 8, 4},
        {1, 8, 5}, {2, 8, 5}, {3, 8, 5},
        {1, 8, 6}, {2, 8, 6}, {3, 8, 6},
        {1, 9, 4}, {2, 9, 4}, {3, 9, 4},
        {1, 9, 5}, {2, 9, 5}, {3, 9, 5},
        {1, 9, 6}, {2, 9, 6}, {3, 9, 6},
    };

    int idx = 0;
    while (cp.next()) {
        EXPECT(output == expected_output[idx++]);
    }
    idx = 0;
    while (cp.next()) {
        EXPECT(output == expected_output[idx++]);
    }
}

CASE("Test unit-sized vectors") {
    std::vector<int> output(3);

    std::vector<int> v1 {1, 2, 3};
    std::vector<int> v2 {4};
    std::vector<int> v3 {7, 8, 9};

    dasi::util::CartesianProduct<std::vector<int>> cp;

    cp.append(v1, output[0]);
    cp.append(v2, output[2]);
    cp.append(v3, output[1]);

    std::vector<std::vector<int>> expected_output {
        {1, 7, 4}, {2, 7, 4}, {3, 7, 4},
        {1, 8, 4}, {2, 8, 4}, {3, 8, 4},
        {1, 9, 4}, {2, 9, 4}, {3, 9, 4},
    };

    int idx = 0;
    while (cp.next()) {
        EXPECT(output == expected_output[idx++]);
    }
    idx = 0;
    while (cp.next()) {
        EXPECT(output == expected_output[idx++]);
    }
}

CASE("Test scalar elements") {
    std::vector<int> output(3);

    std::vector<int> v1 {1, 2, 3};
    std::vector<int> v3 {7, 8, 9};

    dasi::util::CartesianProduct<std::vector<int>> cp;

    cp.append(v1, output[0]);
    cp.append(int(4), output[2]);
    cp.append(v3, output[1]);

    std::vector<std::vector<int>> expected_output {
        {1, 7, 4}, {2, 7, 4}, {3, 7, 4},
        {1, 8, 4}, {2, 8, 4}, {3, 8, 4},
        {1, 9, 4}, {2, 9, 4}, {3, 9, 4},
    };

    int idx = 0;
    while (cp.next()) {
        EXPECT(output == expected_output[idx++]);
    }
    idx = 0;
    while (cp.next()) {
        EXPECT(output == expected_output[idx++]);
    }
}

CASE("Test all unit-sized vectors") {
    std::vector<int> output(3);

    std::vector<int> v1 {1};
    std::vector<int> v2 {4};
    std::vector<int> v3 {7};

    dasi::util::CartesianProduct<std::vector<int>> cp;

    cp.append(v1, output[0]);
    cp.append(v2, output[2]);
    cp.append(v3, output[1]);

    std::vector<std::vector<int>> expected_output {
        {1, 7, 4}
    };

    int idx = 0;
    while (cp.next()) {
        EXPECT(output == expected_output[idx++]);
    }
    idx = 0;
    while (cp.next()) {
        EXPECT(output == expected_output[idx++]);
    }
}

CASE("Test all scalar elements") {
    std::vector<int> output(3);

    dasi::util::CartesianProduct<std::vector<int>> cp;

    cp.append(int(1), output[0]);
    cp.append(int(4), output[2]);
    cp.append(int(7), output[1]);

    std::vector<std::vector<int>> expected_output {
       {1, 7, 4}
    };

    int idx = 0;
    while (cp.next()) {
        EXPECT(output == expected_output[idx++]);
    }
    idx = 0;
    while (cp.next()) {
        EXPECT(output == expected_output[idx++]);
    }
}

CASE("Empty lists are invalid") {
    std::vector<int> output(3);

    std::vector<int> v1 {1, 2, 3};
    std::vector<int> v2 {};
    std::vector<int> v3 {7, 8, 9};

    dasi::util::CartesianProduct<std::vector<int>> cp;

    cp.append(v1, output[0]);
    EXPECT_THROWS_AS(cp.append(v2, output[2]), dasi::util::BadValue);
}

CASE("Entries are mandatory") {
    dasi::util::CartesianProduct<std::vector<int>> cp;
    EXPECT_THROWS_AS(cp.next(), dasi::util::SeriousBug);
}

CASE("Testing a non-iterable cartesian product") {
    std::vector<int> output(3);

    dasi::util::CartesianProduct<int> cp;

    cp.append(int(1), output[0]);
    cp.append(int(4), output[2]);
    cp.append(int(7), output[1]);

    std::vector<std::vector<int>> expected_output {
        {1, 7, 4}
    };

    int idx = 0;
    while (cp.next()) {
        EXPECT(output == expected_output[idx++]);
    }
    idx = 0;
    while (cp.next()) {
        EXPECT(output == expected_output[idx++]);
    }
}

CASE("Works with strings") {
    std::vector<std::string> output(3);

    std::vector<std::string> v1 {"aa", "bb", "cc"};
    std::vector<std::string> v3 {"dd", "ee", "ff"};

    dasi::util::CartesianProduct<std::vector<std::string>> cp;

    cp.append(v1, output[0]);
    cp.append("gg", output[2]);
    cp.append(v3, output[1]);

    std::vector<std::vector<std::string>> expected_output {
        {"aa", "dd", "gg"}, {"bb", "dd", "gg"}, {"cc", "dd", "gg"},
        {"aa", "ee", "gg"}, {"bb", "ee", "gg"}, {"cc", "ee", "gg"},
        {"aa", "ff", "gg"}, {"bb", "ff", "gg"}, {"cc", "ff", "gg"},
    };

    int idx = 0;
    while (cp.next()) {
        EXPECT(output == expected_output[idx++]);
    }
    idx = 0;
    while (cp.next()) {
        EXPECT(output == expected_output[idx++]);
    }
}


CASE("Works with c-style strings") {
    std::vector<const char*> output(3);

    std::vector<const char*> v1 {"aa", "bb", "cc"};
    std::vector<const char*> v3 {"dd", "ee", "ff"};

    dasi::util::CartesianProduct<std::vector<const char*>> cp;

    cp.append(v1, output[0]);
    cp.append("gg", output[2]);
    cp.append(v3, output[1]);

    std::vector<std::vector<std::string>> expected_output {
        {"aa", "dd", "gg"}, {"bb", "dd", "gg"}, {"cc", "dd", "gg"},
        {"aa", "ee", "gg"}, {"bb", "ee", "gg"}, {"cc", "ee", "gg"},
        {"aa", "ff", "gg"}, {"bb", "ff", "gg"}, {"cc", "ff", "gg"},
    };

    int idx = 0;
    while (cp.next()) {
        for (int i = 0; i < output.size(); ++i) EXPECT(output[i] == expected_output[idx][i]);
        ++idx;
    }
    idx = 0;
    while (cp.next()) {
        for (int i = 0; i < output.size(); ++i) EXPECT(output[i] == expected_output[idx][i]);
        ++idx;
    }
}


CASE("Works with string_viewss") {
    std::vector<std::string_view> output(3);

    std::vector<std::string> v1 {"aa", "bb", "cc"};
    std::string v2("gg");
    std::vector<std::string> v3 {"dd", "ee", "ff"};

    dasi::util::CartesianProduct<std::vector<std::string>, std::string_view> cp;

    cp.append(v1, output[0]);
    cp.append(v2, output[2]);
    cp.append(v3, output[1]);

    std::vector<std::vector<std::string_view>> expected_output {
        {"aa", "dd", "gg"}, {"bb", "dd", "gg"}, {"cc", "dd", "gg"},
        {"aa", "ee", "gg"}, {"bb", "ee", "gg"}, {"cc", "ee", "gg"},
        {"aa", "ff", "gg"}, {"bb", "ff", "gg"}, {"cc", "ff", "gg"},
    };

    int idx = 0;
    while (cp.next()) {
        EXPECT(output == expected_output[idx++]);
    }
    idx = 0;
    while (cp.next()) {
        EXPECT(output == expected_output[idx++]);
    }
}

CASE("Works with non-iterable string types") {
    std::vector<std::string> output(3);

    dasi::util::CartesianProduct<std::string> cp;

    cp.append("testing"s, output[0]);
    cp.append("gnitset"s, output[2]);
    cp.append("again another string"s, output[1]);

    std::vector<std::vector<std::string>> expected_output {
        {"testing", "again another string", "gnitset"}
    };

    int idx = 0;
    while (cp.next()) {
        EXPECT(output == expected_output[idx++]);
    }
    idx = 0;
    while (cp.next()) {
        EXPECT(output == expected_output[idx++]);
    }
}

CASE("Works with non-iterable string types") {
    std::vector<std::string_view> output(3);

    dasi::util::CartesianProduct<const char*, std::string_view> cp;

    cp.append("testing", output[0]);
    cp.append("gnitset", output[2]);
    cp.append("again another string", output[1]);

    std::vector<std::vector<std::string>> expected_output {
        {"testing", "again another string", "gnitset"}
    };

    int idx = 0;
    while (cp.next()) {
        for (int i = 0; i < output.size(); ++i) EXPECT(output[i] == expected_output[idx][i]);
        ++idx;
    }
    idx = 0;
    while (cp.next()) {
        for (int i = 0; i < output.size(); ++i) EXPECT(output[i] == expected_output[idx][i]);
        ++idx;
    }
}

CASE("Works with non-iterable string_view types") {
    std::vector<std::string_view> output(3);

    dasi::util::CartesianProduct<std::string, std::string_view> cp;

    std::string v1("testing");
    std::string v2("gnitset");
    std::string v3("again another string");

    cp.append(v1, output[0]);
    cp.append(v2, output[2]);
    cp.append(v3, output[1]);

    std::vector<std::vector<std::string_view>> expected_output {
        {"testing", "again another string", "gnitset"}
    };

    int idx = 0;
    while (cp.next()) {
        EXPECT(output == expected_output[idx++]);
    }
    idx = 0;
    while (cp.next()) {
        EXPECT(output == expected_output[idx++]);
    }
}

//----------------------------------------------------------------------------------------------------------------------

int main(int argc, char** argv) {
    return ::dasi::util::run_tests();
}