
#pragma once

#include <cstddef>

namespace dasi {

class SplitReferenceKey;

//----------------------------------------------------------------------------------------------------------------------

class ArchiveVisitor {

public: // methods

    ArchiveVisitor(const void* data, size_t length);
    ~ArchiveVisitor() = default;

    void firstLevel(const SplitReferenceKey& key) {}
    void secondLevel(const SplitReferenceKey& key) {}
    void thirdLevel(const SplitReferenceKey& key) {}

private: // members

    const void* data_;
    size_t length_;
};

//----------------------------------------------------------------------------------------------------------------------

}