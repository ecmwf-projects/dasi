
#include "dasi/core/AggregatedReadHandle.h"

#include "dasi/util/Exceptions.h"

#include <iostream>
#include <ostream>

namespace dasi::core {

AggregatedReadHandle::AggregatedReadHandle(const std::vector<ReadHandle*>& handles) :
    handles_(handles.begin(), handles.end()),
    current_(handles_.begin()) {}

AggregatedReadHandle::~AggregatedReadHandle() {
    if (closer_) {
        std::cerr << "ERROR: " << *this << " has not been closed" << std::endl;
    }
}

size_t AggregatedReadHandle::read(void* buf, size_t len) {

    if (current_ == handles_.end()) {
        return 0;
    }

    size_t bytes_read = 0;
    auto cur = static_cast<char*>(buf);
    
    do {
        size_t chunk = (*current_)->read(static_cast<void*>(cur), len - bytes_read);
        cur += chunk;
        bytes_read += chunk;

        if (bytes_read < len) {
            ++current_;
            if (current_ == handles_.end()) {
                return bytes_read;
            }
            openInternal();
        }
    } while (bytes_read < len);

    return bytes_read;
}

void AggregatedReadHandle::open() {
    ASSERT(!closer_);
    openInternal();
}

void AggregatedReadHandle::close() {
    closer_.reset();
}

void AggregatedReadHandle::openInternal() {
    (*current_)->open();
    closer_.reset(new util::AutoCloser(**current_));
}

void AggregatedReadHandle::print(std::ostream& s) const {
    s << "AggregatedReadHandle";
}

}