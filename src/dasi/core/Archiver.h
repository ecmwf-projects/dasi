
#pragma once

#include "dasi/core/Schema.h"
#include "dasi/core/DB.h"
#include "dasi/core/OrderedReferenceKey.h"

#include "dasi/util/LRUMap.h"


namespace dasi::api {
class Config;
}

namespace dasi::core {

class SplitReferenceKey;
class ArchiveVisitor;

//----------------------------------------------------------------------------------------------------------------------

class Archiver {

public: // methods

    Archiver(const api::Config& config, const Schema& schema, int lruSize=20);
    ~Archiver() = default;

    void archive(const api::Key& key, const void* data, size_t length);

    DB& database(const OrderedReferenceKey& dbkey);

private: // members

    const api::Config& config_;
    const Schema& schema_;

    util::LRUMap<api::Key, DB, std::less<>> databases_;
};

//----------------------------------------------------------------------------------------------------------------------

}