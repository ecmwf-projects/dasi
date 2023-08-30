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

#include "dasi/api/Dasi.h"

#include "helper.h"

#include "eckit/filesystem/LocalPathName.h"

namespace dasi::testing {

//----------------------------------------------------------------------------------------------------------------------

CASE("testing dasi archive") {
    TempDirectory tempDir(eckit::LocalPathName::cwd() + "/tmp.DASI", false);

    simpleWrite(tempDir, "simple_schema", SIMPLE_SCHEMA);

    dasi::Query query {{"key1", {"value1"}}, {"key2", {"value2"}}, {"key3", {"value3"}}};

    const auto cfg = simpleConfig(tempDir, "simple_schema");

    LOG_D("--- [ARCHIVE] ---");

    dasi::Dasi dasi(cfg.c_str());

    const auto keys = KeySet({"value3b1", "value3b2", "value3b3", "value3b4"});

    for (auto&& key : keys) {
        const std::string data = "DASI PURGE TEST DATA " + key.get("key3b");
        dasi.archive(key, data.data(), data.size());
    }

    dasi.flush();

    LOG_D("--- [LIST] ---");

    size_t count = 0;
    for (auto&& item : dasi.list(query)) { count++; }
    EXPECT(count == 4);

    LOG_D("--- [LIST COUNT: " << count << "] ---");
}

CASE("testing dasi purge") {
    TempDirectory tempDir(eckit::LocalPathName::cwd() + "/tmp.DASI");

    LOG_D("--- [PURGE] ---");

    const auto cfg = simpleConfig(tempDir, "simple_schema");

    dasi::Dasi dasi(cfg.c_str());

    dasi::Query query {{"key1", {"value1"}}, {"key2", {"value2"}}, {"key3", {"value3"}}};

    auto purged = dasi.purge(query, true, true);

    size_t count = 0;
    for (auto&& item : purged) {
        LOG_D("PURGED: " << item);
        count++;
    }

    LOG_D("--- [PURGED: " << count << "] ---");
}

}  // namespace dasi::testing

//----------------------------------------------------------------------------------------------------------------------

int main(int argc, char** argv) {
    eckit::Main::initialise(argc, argv);

    int err = 0;

    {
        dasi::testing::DasiForker archiver;
        archiver.start();
        err += archiver.runTest("testing dasi archive");
        err += archiver.runTest("testing dasi archive");
    }

    {
        dasi::testing::DasiForker purger;
        purger.start();
        err += purger.runTest("testing dasi purge");
    }

    return err;
}
