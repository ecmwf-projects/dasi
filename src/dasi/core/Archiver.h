
#pragma once

#include "dasi/core/Schema.h"
#include "dasi/core/DB.h"

#include "dasi/util/LRUMap.h"


namespace dasi::core {

class SplitReferenceKey;

//----------------------------------------------------------------------------------------------------------------------

class Archiver {

public: // methods

    Archiver(const Schema& schema, int lruSize=20);
    ~Archiver() = default;

    void archive(const api::Key& key, const void* data, size_t length);

private: // members

    const Schema& schema_;

    util::LRUMap<api::Key, DB> databases_;
};

//----------------------------------------------------------------------------------------------------------------------

}