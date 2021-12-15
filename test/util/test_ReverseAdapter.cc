
#include "dasi/util/Test.h"

#include "dasi/util/ReverseAdapter.h"


CASE("Test const access to elements") {

    std::vector<int> values { 1111, 2222, 3333, 4444, 5555 };
    std::vector<int> expected { 5555, 4444, 3333, 2222, 1111 };

    // Check that this works in a range-based for loop

    {
        size_t count = 0;
        for (const auto& val: dasi::util::ReverseAdapter(values)) {
            EXPECT(val == expected[count++]);
        }
        EXPECT(expected.size() == count);
    }

    // Check that we can use types explicitly

    {
        size_t count = 0;
        dasi::util::ReverseAdapter adapt(values);
        dasi::util::ReverseAdapter<std::vector<int>>::const_iterator it;
        dasi::util::ReverseAdapter<std::vector<int>>::const_iterator end;

        for (it = adapt.begin(), end = adapt.end(); it != end; ++it) {
            EXPECT(*it == expected[count++]);
        }
        EXPECT(expected.size() == count);
    }

    // Check that we can use the cbegin/cend accessors

    {
        size_t count = 0;
        dasi::util::ReverseAdapter adapt(values);
        dasi::util::ReverseAdapter<std::vector<int>>::const_iterator it;
        dasi::util::ReverseAdapter<std::vector<int>>::const_iterator end;

        for (it = adapt.cbegin(), end = adapt.cend(); it != end; ++it) {
            EXPECT(*it == expected[count++]);
        }
        EXPECT(expected.size() == count);
    }
}

CASE("Test mutable access to elements") {

    std::vector<int> values { 1111, 2222, 3333, 4444, 5555 };
    std::vector<int> expected { 5555, 4444, 3333, 2222, 1111 };

    std::vector<int> newvals { 1119, 2229, 3339, 4449, 5559 };
    std::vector<int> newexpected { 5559, 4449, 3339, 2229, 1119 };

    std::vector<int> newvals2 { 1118, 2228, 3338, 4448, 5558 };
    std::vector<int> newexpected2 { 5558, 4448, 3338, 2228, 1118 };

    // Set values in a range-based for-loop

    {
        size_t count = 0;
        for (auto& val: dasi::util::ReverseAdapter(values)) {
            EXPECT(val == expected[count]);
            val = newvals[count++];
        }
        EXPECT(expected.size() == count);
    }

    // Check values in a range-based for-loop

    {
        size_t count = 0;
        for (const auto& val: values) {
            EXPECT(val == newexpected[count++]);
        }
        EXPECT(newexpected.size() == count);
    }


    // Check that we can use types explicitly

    {
        size_t count = 0;
        dasi::util::ReverseAdapter adapt(values);
        dasi::util::ReverseAdapter<std::vector<int>>::iterator it;
        dasi::util::ReverseAdapter<std::vector<int>>::iterator end;

        for (it = adapt.begin(), end = adapt.end(); it != end; ++it) {
            EXPECT(*it == newvals[count]);
            *it = newvals2[count++];
        }
        EXPECT(expected.size() == count);
    }

    // Check values in a range-based for-loop

    {
        size_t count = 0;
        for (const auto& val: values) {
            EXPECT(val == newexpected2[count++]);
        }
        EXPECT(newexpected2.size() == count);
    }
}

CASE("Test that we can reverse the reversed values") {

    std::vector<int> values { 1111, 2222, 3333, 4444, 5555 };

    // Check that this works in a range-based for loop

    {
        size_t count = 0;
        dasi::util::ReverseAdapter adapt(values);
        for (const auto& val: dasi::util::ReverseAdapter<decltype(adapt)>(adapt)) {
            EXPECT(val == values[count++]);
        }
        EXPECT(values.size() == count);
    }

    // Check that we can use types explicitly

    {
        size_t count = 0;
        dasi::util::ReverseAdapter radapt(values);
        dasi::util::ReverseAdapter<decltype(radapt)> adapt(radapt);
        dasi::util::ReverseAdapter<dasi::util::ReverseAdapter<std::vector<int>>>::const_iterator it;
        dasi::util::ReverseAdapter<dasi::util::ReverseAdapter<std::vector<int>>>::const_iterator end;

        for (it = adapt.begin(), end = adapt.end(); it != end; ++it) {
            EXPECT(*it == values[count++]);
        }
        EXPECT(values.size() == count);
    }

    // Check that we can use cbegin/cend explicitly

    {
        size_t count = 0;
        dasi::util::ReverseAdapter radapt(values);
        dasi::util::ReverseAdapter<decltype(radapt)> adapt(radapt);
        dasi::util::ReverseAdapter<dasi::util::ReverseAdapter<std::vector<int>>>::const_iterator it;
        dasi::util::ReverseAdapter<dasi::util::ReverseAdapter<std::vector<int>>>::const_iterator end;

        for (it = adapt.cbegin(), end = adapt.cend(); it != end; ++it) {
            EXPECT(*it == values[count++]);
        }
        EXPECT(values.size() == count);
    }
}


CASE("Test mutable access to double reversed elements") {

    std::vector<int> values { 1111, 2222, 3333, 4444, 5555 };
    std::vector<int> newvals { 1119, 2229, 3339, 4449, 5559 };
    std::vector<int> newvals2 { 1118, 2228, 3338, 4448, 5558 };

    // Set values in a range-based for-loop

    {
        size_t count = 0;
        dasi::util::ReverseAdapter radapt(values);
        dasi::util::ReverseAdapter<decltype(radapt)> adapt(radapt);
        for (auto& val : adapt) {
            EXPECT(val == values[count]);
            val = newvals[count++];
        }
        EXPECT(values.size() == count);
    }

    // Check values in a range-based for-loop

    {
        size_t count = 0;
        for (const auto& val: values) {
            EXPECT(val == newvals[count++]);
        }
        EXPECT(newvals.size() == count);
    }


    // Check that we can use types explicitly

    {
        size_t count = 0;
        dasi::util::ReverseAdapter radapt(values);
        dasi::util::ReverseAdapter<decltype(radapt)> adapt(radapt);
        dasi::util::ReverseAdapter<dasi::util::ReverseAdapter<std::vector<int>>>::iterator it;
        dasi::util::ReverseAdapter<dasi::util::ReverseAdapter<std::vector<int>>>::iterator end;

        for (it = adapt.begin(), end = adapt.end(); it != end; ++it) {
            EXPECT(*it == newvals[count]);
            *it = newvals2[count++];
        }
        EXPECT(newvals.size() == count);
    }

    // Check values in a range-based for-loop

    {
        size_t count = 0;
        for (const auto& val: values) {
            EXPECT(val == newvals2[count++]);
        }
        EXPECT(newvals2.size() == count);
    }
}


int main(int argc, char** argv) {
    return ::dasi::util::run_tests();
}