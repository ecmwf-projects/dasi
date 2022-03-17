
#pragma once

#include "dasi/api/ObjectLocation.h"

#include <memory>

namespace dasi::api {
class Config;
}

namespace dasi::core {

class SplitReferenceKey;
class OrderedReferenceKey;
class Catalogue;

//----------------------------------------------------------------------------------------------------------------------

class DB {

public: // methods

    DB(const OrderedReferenceKey& dbkey, const api::Config& config, bool reader);
    DB(DB&& rhs) noexcept;
    virtual ~DB();

    void archive(const SplitReferenceKey& key, const void* data, size_t length);
    api::ObjectLocation retrieve(const SplitReferenceKey& key);
    bool exists(const SplitReferenceKey& key);

private: // members

    std::unique_ptr<Catalogue> catalogue_;
};

//----------------------------------------------------------------------------------------------------------------------

}