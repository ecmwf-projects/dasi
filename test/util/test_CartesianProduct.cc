
#include "dasi/util/Test.h"

#include "dasi/util/CartesianProduct.h"
#include "dasi/util/ContainerIostream.h"
#include "dasi/util/Exceptions.h"

//----------------------------------------------------------------------------------------------------------------------

CASE("Test output ordering") {
    std::vector<int> output(3);

    std::vector<int> v1 {1, 2, 3};
    std::vector<int> v2 {4, 5, 6};
    std::vector<int> v3 {7, 8, 9};

    dasi::CartesianProduct<std::vector<int>> cp;

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

    dasi::CartesianProduct<std::vector<int>> cp;

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

    dasi::CartesianProduct<std::vector<int>> cp;

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

    dasi::CartesianProduct<std::vector<int>> cp;

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

    dasi::CartesianProduct<std::vector<int>> cp;

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

    dasi::CartesianProduct<std::vector<int>> cp;

    cp.append(v1, output[0]);
    EXPECT_THROWS_AS(cp.append(v2, output[2]), dasi::BadValue);
}

CASE("Entries are mandatory") {
    dasi::CartesianProduct<std::vector<int>> cp;
    EXPECT_THROWS_AS(cp.next(), dasi::SeriousBug);
}



//----------------------------------------------------------------------------------------------------------------------

int main(int argc, char** argv) {
    return ::dasi::run_tests();
}