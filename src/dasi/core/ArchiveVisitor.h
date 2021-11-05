
#pragma once

#include <cstddef>

namespace dasi {

//----------------------------------------------------------------------------------------------------------------------

class ArchiveVisitor {

public: // methods

    ArchiveVisitor(const void* data, size_t length);
    ~ArchiveVisitor() = default;

    void firstLevel() {}
    void secondLevel() {}
    void thirdLevel() {}

private: // members

    const void* data_;
    size_t length_;
};

//----------------------------------------------------------------------------------------------------------------------

}