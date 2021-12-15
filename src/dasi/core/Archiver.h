
#pragma once

#include "dasi/core/Schema.h"


namespace dasi::core {

class SplitReferenceKey;

//----------------------------------------------------------------------------------------------------------------------

class Archiver {

public: // methods

    Archiver(const Schema& schema);
    ~Archiver() = default;

    void archive(const api::Key& key, const void* data, size_t length);

private: // members

    const Schema& schema_;
};

//----------------------------------------------------------------------------------------------------------------------

}