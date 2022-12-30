/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "Dasi.h"

#include "dasi/lib/LibDasi.h"
#include "dasi/impl/ListGeneratorImpl.h"

#include "fdb5/api/FDB.h"
#include "fdb5/api/helpers/FDBToolRequest.h"
#include "fdb5/config/Config.h"
#include "fdb5/rules/Schema.h"

#include "eckit/config/YAMLConfiguration.h"
#include "eckit/exception/Exceptions.h"
#include "eckit/log/Log.h"
#include "eckit/runtime/Main.h"

#include "metkit/mars/MarsRequest.h"

#include <memory>


namespace dasi {

//----------------------------------------------------------------------------------------------------------------------

namespace {

// Eckit initialisation helpers

void initialise_eckit_once() {
    static bool initialised = false;
    if (!initialised) {
        if (!eckit::Main::ready()) {
            static const char* argv[2] = {"dasi", nullptr};
            eckit::Main::initialise(1, const_cast<char**>(argv));
        }
        initialised = true;
    }
}

// This is implemented as a class, such that it can be placed as the first
// initialiser of DamiImpl, before any of the data members.
struct EckitMainHelper {
    EckitMainHelper() {
        initialise_eckit_once();
    }
};

// Dasi Implementation construction helpers

eckit::LocalConfiguration parse_application_config(const char* application_config) {

    if (application_config) {
        std::istringstream app_ss(application_config);
        return eckit::LocalConfiguration{eckit::YAMLConfiguration(app_ss)};
    }

    return eckit::LocalConfiguration{};
}

fdb5::Config construct_config(const char* dasi_config, const char* application_config) {

    // Is the dasi_configation a raw yaml, or is it a path?
    // FIXME: TODO: This behaves badly if there is a _path_ that doesn't exist (fails on parsing)

    eckit::PathName config_path(dasi_config);
    std::string yaml_config;
    if (config_path.exists()) {
        std::unique_ptr<eckit::DataHandle> dh(config_path.fileHandle());
        size_t sz = dh->size();
        dh->openForRead();
        eckit::AutoClose closer(*dh);
        yaml_config.resize(sz);
        ASSERT(dh->read(&yaml_config[0], sz) == sz);
    } else {
        yaml_config = dasi_config;
    }

    ASSERT(dasi_config);
    LOG_DEBUG_LIB(LibDasi) << "Dasi Configuration: " << eckit::newl;
    LOG_DEBUG_LIB(LibDasi) << yaml_config << eckit::newl << std::endl;

    LOG_DEBUG_LIB(LibDasi) << "Application Configuration:";
    if (application_config) {
        LOG_DEBUG_LIB(LibDasi) << eckit::newl;
        LOG_DEBUG_LIB(LibDasi) << application_config << eckit::newl << std::endl;
    } else {
        LOG_DEBUG_LIB(LibDasi) << " <default>" << std::endl;
    }

    std::istringstream dasi_ss(yaml_config);
    eckit::YAMLConfiguration cfg(dasi_ss);

    if (cfg.has("schema")) {
        LOG_DEBUG_LIB(LibDasi) << "Dasi Schema: " << eckit::newl;
        LOG_DEBUG_LIB(LibDasi) << fdb5::Schema{cfg.getString("schema")} << eckit::newl << std::endl;
    }

    fdb5::Config retval{cfg, parse_application_config(application_config)};

    // By default, we specify local type, so we don't have to expose all of the FDB frontend-routing at this time
    if (!retval.has("type")) {
        retval.set("type", "local");
    }

    return retval;
}

}

//----------------------------------------------------------------------------------------------------------------------

/// @note We use an internal implementation for two reasons:
///       (i) It ensures that the interface is kept as clean as possible
///      (ii) It ensures that no FDB headers are exposed through DASI

class DasiImpl {

public: // methods

    DasiImpl(const char* dasi_config, const char* application_config) :
        mainHelper_(),
        fdb_(construct_config(dasi_config, application_config)) {}

    void archive(const Key& key, const void* data, size_t length) {
        fdb5::Key fdb_key;
        for (const auto& kv : key) {
            fdb_key.set(kv.first, kv.second);
        }
        fdb_.archive(fdb_key, data, length);
    }

    ListGenerator list(const Query& query) {
        bool deduplicate = true;
        auto&& iter = fdb_.list(fdb5::FDBToolRequest(queryToMarsRequest(query)), deduplicate);

        return ListGenerator(std::make_unique<ListGeneratorImpl>(std::move(iter)));
    }

    std::unique_ptr<eckit::DataHandle> retrieve(const Query& query) {
        eckit::DataHandle* dh = fdb_.retrieve(queryToMarsRequest(query));
        return std::unique_ptr<eckit::DataHandle>(dh);
    }

    void flush() {
        fdb_.flush();
    }

private: // methods

    metkit::mars::MarsRequest queryToMarsRequest(const Query& query) {
        metkit::mars::MarsRequest rq("retrieve");
        for (const auto& kv : query) {
            rq.values(kv.first, kv.second);
        }
        return rq;
    }

private: // members

    // DASI may well be the first eckit-like entry point to the application.
    // If it is, then make sure that everything goes nicely.
    EckitMainHelper mainHelper_;

    // The real deal, this is where most of the underlying work is done!
    fdb5::FDB fdb_;
};

//----------------------------------------------------------------------------------------------------------------------

Dasi::Dasi(const char* dasi_config, const char* application_config) :
    impl_(new DasiImpl(dasi_config, application_config)) {}


// Destructor must be implemented in .cc, not as default in header, as DasiImpl
// is only forward declared, whereas it is available in this translation unit
Dasi::~Dasi() {}

void Dasi::archive(const Key& key, const void* data, size_t length) {
    ASSERT(impl_);
    impl_->archive(key, data, length);
}

ListGenerator Dasi::list(const Query& query) {
    ASSERT(impl_);
    return impl_->list(query);
}

std::unique_ptr<eckit::DataHandle> Dasi::retrieve(const Query& query) {
    ASSERT(impl_);
    return impl_->retrieve(query);
}

void Dasi::flush() {
    ASSERT(impl_);
    impl_->flush();
}

//----------------------------------------------------------------------------------------------------------------------

} // namespace dasi

