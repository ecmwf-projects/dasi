
#include "dasi/api/RetrieveResult.h"

#include "dasi/core/AggregatedReadHandle.h"

namespace dasi::api {

//----------------------------------------------------------------------------------------------------------------------

RetrieveResult::~RetrieveResult() {
    for (auto val : values_) {
        delete val;
    }
}

void RetrieveResult::append(const Key& key, item_type value) {
    keys_.push_back(key);
    values_.push_back(value);
}

ReadHandle* RetrieveResult::toHandle() {
    auto handle = new dasi::core::AggregatedReadHandle(values_);
    values_.clear();
    return handle;
}

//----------------------------------------------------------------------------------------------------------------------

}