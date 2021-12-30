
#include "dasi/core/Archiver.h"

using dasi::api::Config;

namespace dasi::core {

//----------------------------------------------------------------------------------------------------------------------

class ArchiveVisitor {

public: // methods

    ArchiveVisitor(Archiver& parent, const void* data, size_t length) :
        parent_(parent),
        data_(data),
        length_(length) {}

    void thirdLevel(SplitReferenceKey& key) {
        std::cout << "Third level in archive!!!" << std::endl;
        DB& db = parent_.database(key[0]);
        db.archive(key, data_, length_);
    }

private: // members

    Archiver& parent_;
    const void* data_;
    size_t length_;
};

//----------------------------------------------------------------------------------------------------------------------

Archiver::Archiver(const Config& config, const Schema& schema, int lruSize) :
    config_(config),
    schema_(schema),
    databases_(lruSize) {}

void Archiver::archive(const api::Key& key, const void* data, size_t length) {
    ArchiveVisitor visitor(*this, data, length);
    schema_.walk(key, visitor);
}

DB& Archiver::database(const OrderedReferenceKey& dbkey) {
    auto it = databases_.find(dbkey);
    if (it == databases_.end()) {
        bool reader = false;
        OrderedKey insertKey{dbkey};
        auto ins = databases_.insert(std::move(insertKey), DB(dbkey, config_, reader));
        ASSERT(ins.second);
        it = ins.first;
    }
    return it->second;
}

//----------------------------------------------------------------------------------------------------------------------

}