
///@note Modelled on eckit/testing/Tesh.h

#pragma once

#include "dasi/util/Exceptions.h"
#include "dasi/util/Colour.h"

#include <functional>
#include <string>
#include <sstream>
#include <iostream>
#include <set>

namespace dasi {

//----------------------------------------------------------------------------------------------------------------------

class TestException : public Exception {
public:
    TestException(const std::string& w, const CodeLocation& l) : Exception(w, l) {}
};

//----------------------------------------------------------------------------------------------------------------------

/// A test is defined by a description and a function pointer.

class Test {

public:  // methods
    Test(std::string description, std::function<void()> testFn) :
        description_(std::move(description)), testFn_(std::move(testFn)) {}

    bool run(std::vector<std::string>& failures) {

        bool success = true;

        try {
            testFn_();
        }
        catch (TestException& e) {
            success = false;
            failures.push_back(description());
            std::cerr << Colour::red << "Test \"" << description() << "\" failed: " << e.what()
                      << " @ " << e.location() << Colour::reset << std::endl;
        }
        catch (Exception& e) {
            success = false;
            failures.push_back(description());
            std::cerr << Colour::red << "Test \"" << description()
                      << "\" failed with unhandled Exception: " << e.what() << " @ "
                      << e.location() << Colour::reset << std::endl;
        }
        catch (std::exception& e) {
            success = false;
            failures.push_back(description());
            std::cerr << Colour::red << "Test \"" << description()
                      << "\" failed with unhandled exception: " << e.what() << Colour::reset
                      << std::endl;
        }
        catch (...) {
            success = false;
            failures.push_back(description());
            std::cerr << Colour::red << "Test \"" << description()
                      << "\" failed with unknown unhandled exception." << Colour::reset
                      << std::endl;
        }

        return success;
    }

    [[ nodiscard ]]
    const std::string& description() const {
        return description_;
    }

private:  // members
    std::string description_;
    std::function<void()> testFn_;
};

std::vector<Test>& specification() {
    static std::vector<Test> tests;
    return tests;
}

//----------------------------------------------------------------------------------------------------------------------

/// Tests are registered using a static object, that is initialised before main()

class TestRegister {
public:
    TestRegister(const std::string& description, void (*testFn)()) {
        specification().emplace_back(Test(description, testFn));
    }
};

//----------------------------------------------------------------------------------------------------------------------

inline int run_tests() {

    auto& tests(specification());

    // Keep track of failures
    std::vector<std::string> failures;
    size_t num_tests = tests.size();

    if (num_tests == 0) {
        std::cout << "No test cases to run." << std::endl;
        return -1;
    }

    bool run_all = true;
    std::set<long> runTests;
//    if (::getenv("DASI_TEST_TESTS")) {
//        auto tsts = Translator<std::string, std::vector<long>>()(::getenv("DASI_TEST_TESTS"));
//        runTests.insert(tsts.begin(), tsts.end());
//        run_all = false;
//    }

    for (size_t i = 0; i < num_tests; i++) {

        Test& test(tests[i]);

        if (run_all || runTests.find(i) != runTests.end()) {
            std::cout << "Running case " << i << ": " << test.description() << " ..." << std::endl;
            test.run(failures);
            std::cout << "Completed case " << i << ": " << test.description() << std::endl;
        } else {
            std::cout << "Skipping case " << i << ": " << test.description() << "..." << std::endl;
        }
    }

    for (size_t i = 0; i < failures.size(); i++) {
        std::cout << "\tFAILED: " << failures[i] << std::endl;
    }

    // narrow to the return type of main
    return static_cast<int>(failures.size());
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit

// Helper macros for unique naming

#define UNIQUE_NAME2(name, line) UNIQUE_NAME(name, line)
#define UNIQUE_NAME(name, line) name##line

#define CASE(description)                                                                                           \
    void UNIQUE_NAME2(test_, __LINE__)();                                                                           \
    /* NOLINTNEXTLINE */ \
    static ::dasi::TestRegister UNIQUE_NAME2(test_registration_, __LINE__)(description,                     \
                                                                                   &UNIQUE_NAME2(test_, __LINE__)); \
    void UNIQUE_NAME2(test_, __LINE__)()


#define EXPECT(expr)                                                                 \
    do {                                                                             \
        if (!(expr)) {                                                               \
            throw ::dasi::TestException("Condition failed: " #expr, Here()); \
        }                                                                            \
    } while (false)

#define EXPECT_THROWS_AS(expr, excpt)                                                                        \
    do {                                                                                                     \
        try {                                                                                                \
            expr;                                                                                            \
        }                                                                                                    \
        catch (excpt&) {                                                                                     \
            break;                                                                                           \
        }                                                                                                    \
        throw ::dasi::TestException("Expected exception (" #excpt ")not thrown in: " #expr, Here()); \
    } while (false)


// Convert to TestException
#define EXPECT_NO_THROW(expr)                                                                       \
    do {                                                                                            \
        try {                                                                                       \
            expr;                                                                                   \
        }                                                                                           \
        catch (std::exception & e) {                                                                \
            std::ostringstream oss;                                                                 \
            oss << "Unexpected exception caught: " << e.what();                                     \
            throw ::dasi::TestException(oss.str(), Here());                                 \
        }                                                                                           \
        catch (...) {                                                                               \
            throw ::dasi::TestException("Unexpected and unknown exception caught", Here()); \
        }                                                                                           \
    } while (false)

#define EXPECT_THROWS(expr)                                                                   \
    do {                                                                                      \
        try {                                                                                 \
            expr;                                                                             \
        }                                                                                     \
        catch (...) {                                                                         \
            break;                                                                            \
        }                                                                                     \
        throw ::dasi::TestException("Exception expected but was not thrown", Here()); \
    } while (false)

//----------------------------------------------------------------------------------------------------------------------

