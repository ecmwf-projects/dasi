
#pragma once

#include "dasi/core/Schema.h"
#include "dasi/core/DB.h"
#include "dasi/core/OrderedKey.h"

#include "dasi/util/LRUMap.h"


namespace dasi::api {
class Config;
class Query;
class Result;
}

namespace dasi::core {

class SplitReferenceKey;
class OrderedReferenceKey;

//----------------------------------------------------------------------------------------------------------------------

class Retriever {

public: // methods

    Retriever(const api::Config& config, const Schema& schema, int lruSize=20);
    ~Retriever() = default;

    api::Result retrieve(const api::Query& query);

    DB& database(const OrderedReferenceKey& dbkey);

private: // members

    const api::Config& config_;
    const Schema& schema_;

    util::LRUMap<core::OrderedKey, DB, std::less<>> databases_;
};

//----------------------------------------------------------------------------------------------------------------------

}