
#include "dasi/api/Query.h"

#include "dasi/core/Retriever.h"
#include "dasi/core/AggregatedHandle.h"

using dasi::api::Config;

namespace dasi::core {

//----------------------------------------------------------------------------------------------------------------------

class RetrieveVisitor {

public: // methods

    RetrieveVisitor(Retriever& parent, std::vector<api::Handle*>& handles) :
        parent_(parent),
        handles_(handles) {}

    void thirdLevel(SplitReferenceKey& key) {
        std::cout << "Third level in retrieve!!!" << std::endl;
        DB& db = parent_.database(key[0]);
        auto result = db.retrieve(key);
        handles_.push_back(result);
    }

private: // members

    Retriever& parent_;
    std::vector<api::Handle*>& handles_;
};

//----------------------------------------------------------------------------------------------------------------------

Retriever::Retriever(const Config& config, const Schema& schema, int lruSize) :
    config_(config),
    schema_(schema),
    databases_(lruSize) {}

api::Handle* Retriever::retrieve(const api::Query& query) {
    std::vector<api::Handle*> handles;
    RetrieveVisitor visitor(*this, handles);
    schema_.walk(query, visitor);
    return new AggregatedHandle(handles);
}

DB& Retriever::database(const OrderedReferenceKey& dbkey) {
    auto it = databases_.find(dbkey);
    if (it == databases_.end()) {
        bool reader = true;
        OrderedKey insertKey{dbkey};
        auto ins = databases_.insert(std::move(insertKey), DB(dbkey, config_, reader));
        ASSERT(ins.second);
        it = ins.first;
    }
    return it->second;
}

//----------------------------------------------------------------------------------------------------------------------

}