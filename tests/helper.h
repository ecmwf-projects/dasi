/*
 * Copyright 2023- European Centre for Medium-Range Weather Forecasts (ECMWF).
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     https://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/// @author Metin Cakircali
/// @date Aug 2023

#pragma once

#include "dasi/api/Key.h"
#include "dasi/api/detail/ListDetail.h"
#include "dasi/lib/LibDasi.h"

#include "dasi/api/dasi_c.h"

#include "eckit/exception/Exceptions.h"
#include "eckit/testing/Test.h"

#include "eckit/runtime/ProcessControler.h"
#include "eckit/runtime/Monitor.h"

#include <unistd.h>    // mkdtemp
#include <filesystem>  // c++17
#include <fstream>

#define LOG_S(msg) eckit::Log::status() << msg << std::endl
#define LOG_I(msg) eckit::Log::info() << msg << std::endl
#define LOG_D(msg) LOG_DEBUG_LIB(LibDasi) << msg << std::endl

#define CHECK_RETURN(x) EXPECT((x) == DASI_SUCCESS)

namespace fs = std::filesystem;

namespace dasi::testing {

//----------------------------------------------------------------------------------------------------------------------

constexpr const char SIMPLE_SCHEMA[] = "[ key1, key2, key3\n"
                                       "  [ key1a, key2a, key3a\n"
                                       "    [ key1b, key2b, key3b ]]]\n";

void simpleWrite(const fs::path& path, const char* filename, const char* data) {
    std::ofstream ofs(path / filename);
    ofs << data;
    ofs.close();
    if (!ofs) { throw eckit::WriteError(path / filename, Here()); }
}

std::string simpleConfig(const fs::path& path, const char* schema) {
    std::ostringstream oss;
    oss << "schema: " << path / schema << "\n";
    oss << "calatogue: toc\n"
           "store: file\n"
           "spaces:\n"
           "- handler: Default\n"
           "  roots:\n";
    oss << "  - path: " << path << "\n";
    return oss.str();
}

//----------------------------------------------------------------------------------------------------------------------

class DasiForker : public eckit::ProcessControler {
    void run() {
        eckit::Monitor::instance().reset();

        // Ensure random state is reset after fork
        ::srand(::getpid() + ::time(nullptr));
        ::srandom(::getpid() + ::time(nullptr));

        LOG_D("Forked pid: " << ::getpid());
    }

public:
    int runTest(const std::string& testName) const {
        std::vector<eckit::testing::Test> tests;
        for (auto&& item : eckit::testing::specification()) {
            if (item.description().compare(testName) == 0) { tests.push_back(item); }
        }
        return eckit::testing::run(tests);
    }

    DasiForker(): eckit::ProcessControler(true) { }
};

//----------------------------------------------------------------------------------------------------------------------

class TempDirectory : public fs::path {
public:
    TempDirectory(const bool remove = true): remove_(remove) {
        std::string temp = fs::temp_directory_path() / "tmp.XXXXXX";

        if (!::mkdtemp(temp.data())) { throw eckit::Exception("Cannot create temporary directory!", Here()); }

        assign(temp);
    }

    TempDirectory(const std::string& basedir, const bool remove = true): remove_(remove) {
        assign(basedir);
        if (!fs::is_directory(*this)) { fs::create_directory(*this); }
    }

    void write(const char* filename, const char* data) { simpleWrite(*this, filename, data); }

    ~TempDirectory() {
        if (remove_) { fs::remove_all(*this); }
    }

private:
    const bool remove_ {false};
};

//----------------------------------------------------------------------------------------------------------------------

class KeySet : public std::set<Key> {
public:
    KeySet() = default;

    KeySet(const std::set<const char*>& values) {
        for (auto&& value : values) {
            this->insert({{"key1", "value1"},
                          {"key2", "value2"},
                          {"key3", "value3"},
                          {"key1a", "value1a"},
                          {"key2a", "value2a"},
                          {"key3a", "value3a"},
                          {"key1b", "value1b"},
                          {"key2b", "value2b"},
                          {"key3b", value}});
        }
    }

    auto lookup(ListGenerator& list) const {
        size_t count = 0;
        for (auto&& item : list) {
            if (this->find(item.key) != this->end()) { count++; }
        }
        return count;
    }

    auto lookup(dasi_list_t* list) const {
        EXPECT(list);

        size_t count = 0;

        int rc;

        while ((rc = dasi_list_next(list)) == DASI_SUCCESS) {
            dasi_key_t* ckey;
            CHECK_RETURN(dasi_list_attrs(list, &ckey, nullptr, nullptr, nullptr, nullptr));
            EXPECT(ckey);

            long keyCount = 0;
            CHECK_RETURN(dasi_key_count(ckey, &keyCount));

            Key key;
            for (long i = 0; i < keyCount; ++i) {
                const char *k, *v;
                CHECK_RETURN(dasi_key_get_index(ckey, i, &k, &v));
                key.set(k, v);
            }

            if (this->find(key) != this->end()) { count++; }

            CHECK_RETURN(dasi_free_key(ckey));
        }

        EXPECT(rc == DASI_ITERATION_COMPLETE);

        return count;
    }
};

}  // namespace dasi::testing
