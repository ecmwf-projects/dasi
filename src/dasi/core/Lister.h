
#pragma once

#include "dasi/api/ListResult.h"

#include "dasi/core/Schema.h"
#include "dasi/core/DB.h"
#include "dasi/core/OrderedKey.h"

#include "dasi/util/LRUMap.h"


namespace dasi::api {
class Config;
class Query;
}

namespace dasi::core {

class OrderedReferenceKey;

//----------------------------------------------------------------------------------------------------------------------

class Lister {

public: // methods

    Lister(const api::Config& config, const Schema& schema, int queueSize=100, int lruSize=20);
    ~Lister() = default;

    api::ListResult list(const api::Query& query);

    DB& database(const OrderedReferenceKey& dbkey);

private: // members

    const api::Config& config_;
    const Schema& schema_;
    const int queueSize_;

    util::LRUMap<core::OrderedKey, DB, std::less<>> databases_;
};

//----------------------------------------------------------------------------------------------------------------------

}