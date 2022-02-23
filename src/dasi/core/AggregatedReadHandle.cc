
#include "dasi/core/AggregatedReadHandle.h"

#include "dasi/util/Exceptions.h"

#include <iostream>
#include <ostream>

namespace dasi::core {

AggregatedReadHandle::AggregatedReadHandle(const std::vector<ReadHandle*>& handles) :
    handles_(handles.begin(), handles.end()),
    current_(handles_.begin()) {}

AggregatedReadHandle::~AggregatedReadHandle() {
    if (!handles_.empty()) {
        std::cerr << "ERROR: " << *this << " has not been closed" << std::endl;
    }
}

size_t AggregatedReadHandle::read(void* buf, size_t len) {

    if (current_ == handles_.end()) {
        return 0;
    }

    size_t nread = 0;
    auto cur = static_cast<char*>(buf);
    
    do {
        size_t chunk = (*current_)->read(static_cast<void*>(cur), len - nread);
        cur += chunk;
        nread += chunk;

        if (nread < len) {
            if (!next()) {
                return nread;
            }
        }
    } while (nread < len);

    return nread;
}

bool AggregatedReadHandle::next() {
    ++current_;
    bool moreData = current_ != handles_.end();
    if (moreData) {
        open();
    }
    return moreData;
}

void AggregatedReadHandle::open() {
    (*current_)->open();
}

void AggregatedReadHandle::close() {
    for (auto& h : handles_) {
        h->close();
    }

    handles_.clear();
}

void AggregatedReadHandle::print(std::ostream& s) const {
    s << "AggregatedReadHandle";
}

}