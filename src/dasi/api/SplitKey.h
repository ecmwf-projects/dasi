
#pragma once

#include "dasi/core/SplitKeyT.h"
#include "dasi/core/OrderedKey.h"
#include "dasi/core/SplitReferenceKey.h"

#include <map>

namespace dasi::api {

//----------------------------------------------------------------------------------------------------------------------

class SplitKey : public core::SplitKeyT<core::OrderedKey> {

public: // methods

    using SplitKeyT::SplitKeyT;

    explicit SplitKey(const core::SplitReferenceKey& rhs) {
        keys_[0] = rhs[0];
        keys_[1] = rhs[1];
        keys_[2] = rhs[2];
    }

    SplitKey& operator=(const core::SplitReferenceKey& rhs) {
        keys_[0] = rhs[0];
        keys_[1] = rhs[1];
        keys_[2] = rhs[2];
        return *this;
    }
};

//----------------------------------------------------------------------------------------------------------------------

}