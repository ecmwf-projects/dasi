
#pragma once

#include <memory>

namespace dasi::api {
class Config;
class ReadHandle;
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
    api::ReadHandle* retrieve(const SplitReferenceKey& key);

private: // members

    std::unique_ptr<Catalogue> catalogue_;
};

//----------------------------------------------------------------------------------------------------------------------

}