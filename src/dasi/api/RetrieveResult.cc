
#include "dasi/api/RetrieveResult.h"

#include "dasi/api/ReadHandle.h"

#include "dasi/core/AggregatedReadHandle.h"

namespace dasi::api {

//----------------------------------------------------------------------------------------------------------------------

RetrieveResult::~RetrieveResult() {
    for (auto&& [key, val] : values_) {
        if (val != nullptr) {
            delete val;
        }
    }
}

void RetrieveResult::append(const Key& key, ReadHandle* value) {
    values_.emplace_back(key, value);
}

ReadHandle* RetrieveResult::toHandle() {
    std::vector<ReadHandle*> handles;
    handles.reserve(values_.size());
    for (auto& kv : values_) {
        handles.push_back(kv.second);
        kv.second = nullptr;
    }
    return new dasi::core::AggregatedReadHandle(handles);
}

//----------------------------------------------------------------------------------------------------------------------

}