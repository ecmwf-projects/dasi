
#include "dasi/util/Test.h"

#include "dasi/util/AsyncGenerator.h"
#include "dasi/util/Exceptions.h"

#include <iostream>
#include <string>
#include <vector>

using namespace dasi::util;

template<typename T>
std::vector<T> exhaust(AsyncGenerator<T>& gen, bool verbose = true) {
    std::vector<T> result;
    while (gen.next()) {
        auto val = gen.get();
        if (verbose) {
            std::cout << "Got " << val << std::endl;
        }
        result.push_back(val);
    }
    return result;
}

template<typename T>
std::vector<T> take(size_t n, AsyncGenerator<T>& gen, bool verbose = true) {
    std::vector<T> result;
    while (gen.next() && result.size() < n) {
        auto val = gen.get();
        if (verbose) {
            std::cout << "Got " << val << std::endl;
        }
        result.push_back(val);
    }
    return result;
}

CASE("Trivial generator") {
    AsyncGenerator<int> vals(10);

    for (int i = 0; i < 5; ++i) {
        vals.push(i);
    }
    vals.finish();

    std::vector<int> expected{0, 1, 2, 3, 4};
    auto actual = exhaust(vals);
    EXPECT(expected == actual);
}

CASE("Empty trivial generator") {
    AsyncGenerator<int> vals(10);

    vals.finish();

    auto actual = exhaust(vals);
    EXPECT(actual.empty());
    EXPECT_THROWS_AS(vals.get(), AssertionFailed);
}

CASE("Simple async generator") {
    AsyncGenerator<char> vals(10, [] (auto& gen) {
        for (char c = 'a'; c < 'e'; ++c) {
            gen.push(c);
        }
    });

    std::vector<char> expected{'a', 'b', 'c', 'd'};
    auto actual = exhaust(vals);
    EXPECT(expected == actual);
}

CASE("Empty async generator") {
    AsyncGenerator<double> vals(10, [] (auto& gen) {});

    auto actual = exhaust(vals);
    EXPECT(actual.empty());
    EXPECT_THROWS_AS(vals.get(), AssertionFailed);
}

CASE("Async generator with more values than the queue can hold") {
    AsyncGenerator<int> vals(5, [] (auto& gen) {
        for (int i = 0; i < 10; ++i) {
            gen.push(i);
        }
    });

    std::vector<int> expected{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    auto actual = exhaust(vals);
    EXPECT(expected == actual);
}

CASE("Infinite async generator") {
    AsyncGenerator<long> vals(5, [] (auto& gen) {
        long i = 0;
        while (true) {
            std::cout << "Push " << i << std::endl;
            gen.push(i++);
        }
    });

    std::vector<long> expected{0, 1, 2, 3, 4, 5, 6};
    auto actual = take(7, vals);
    EXPECT(expected == actual);
}

CASE("Exception in async generator") {
    AsyncGenerator<double> vals(5, [] (auto& gen) {
        double term = 1.;
        double sum = 0.;
        while (true) {
            sum += term;
            gen.push(sum);
            term /= 2.;
            if (term < 0.1) {
                throw UserError("Too small", Here());
            }
        }
    });

    EXPECT_THROWS_AS(exhaust(vals), UserError);
}

CASE("Get from exhausted generator") {
    AsyncGenerator<std::string> vals(5, [] (auto& gen) {
        for (auto&& s : {"this", "is", "a", "test"}) {
            gen.push(std::move(s));
        }
    });

    std::vector<std::string> expected{"this", "is", "a", "test"};
    auto actual = exhaust(vals);
    EXPECT(expected == actual);
    EXPECT_THROWS_AS(vals.get(), AssertionFailed);
}

CASE("Wrapped generator") {
    Generator<int> vals(new AsyncGenerator<int>(10, [] (auto& gen) {
        int x = 3;
        gen.push(x);
        while (x != 1) {
            x = (x % 2 == 0)? x / 2 : 3 * x + 1;
            gen.push(x);
        }
    }));

    std::vector<int> expected{3, 10, 5, 16, 8, 4, 2, 1};
    auto it = expected.cbegin();
    for (auto val : vals) {
        EXPECT(it != expected.cend());
        EXPECT(val == *it);
        ++it;
    }
    EXPECT(it == expected.cend());
}

//----------------------------------------------------------------------------------------------------------------------

int main(int argc, char** argv) {
    return ::dasi::util::run_tests();
}