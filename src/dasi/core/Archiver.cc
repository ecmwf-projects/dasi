
#include "dasi/core/Archiver.h"

namespace dasi::core {

//----------------------------------------------------------------------------------------------------------------------

class ArchiveVisitor {

public: // methods

    ArchiveVisitor(const void* data, size_t length) :
        data_(data),
        length_(length) {}

    void thirdLevel(SplitReferenceKey& key) {
        std::cout << "Third level in archive!!!" << std::endl;
    }

private: // members

    const void* data_;
    size_t length_;
};

//----------------------------------------------------------------------------------------------------------------------

Archiver::Archiver(const Schema& schema) :
    schema_(schema) {}

void Archiver::archive(const api::Key& key, const void* data, size_t length) {
    ArchiveVisitor visitor(data, length);
    schema_.walk(key, visitor);
}

//----------------------------------------------------------------------------------------------------------------------

}