
#include "dasi/api/ReadHandle.h"
#include "dasi/api/Query.h"
#include "dasi/api/RetrieveResult.h"

#include "dasi/core/Retriever.h"

using dasi::api::Config;

namespace dasi::core {

//----------------------------------------------------------------------------------------------------------------------

class RetrieveVisitor {

public: // methods

    RetrieveVisitor(Retriever& parent, api::RetrieveResult& res) :
        parent_(parent),
        res_(res) {}

    void thirdLevel(SplitReferenceKey& key) {
        std::cout << "Third level in retrieve!!!" << std::endl;
        DB& db = parent_.database(key[0]);
        auto result = db.retrieve(key);
        res_.append(api::SplitKey(key), std::move(result));
    }

private: // members

    Retriever& parent_;
    api::RetrieveResult& res_;
};

//----------------------------------------------------------------------------------------------------------------------

Retriever::Retriever(const Config& config, const Schema& schema, int lruSize) :
    config_(config),
    schema_(schema),
    databases_(lruSize) {}

api::RetrieveResult Retriever::retrieve(const api::Query& query) {
    api::RetrieveResult res;
    RetrieveVisitor visitor(*this, res);
    schema_.walk(query, visitor);
    return res;
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