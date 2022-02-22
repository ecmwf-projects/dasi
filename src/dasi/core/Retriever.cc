
#include "dasi/api/Query.h"

#include "dasi/core/Retriever.h"
#include "dasi/core/AggregatedHandle.h"

using dasi::api::Config;

namespace dasi::core {

//----------------------------------------------------------------------------------------------------------------------

class HandleAggregator {

public: // methods

    ~HandleAggregator() {
        for(auto handle : handles_) {
            delete handle;
        }
    }

    void append(api::Handle* handle) {
        handles_.push_back(handle);
    }

    api::Handle* toHandle() {
        auto handle = new AggregatedHandle(handles_);
        handles_.clear();
        return handle;
    }

private: // members

    std::vector<api::Handle*> handles_;
};

//----------------------------------------------------------------------------------------------------------------------

class RetrieveVisitor {

public: // methods

    RetrieveVisitor(Retriever& parent, HandleAggregator& agg) :
        parent_(parent),
        agg_(agg) {}

    void thirdLevel(SplitReferenceKey& key) {
        std::cout << "Third level in retrieve!!!" << std::endl;
        DB& db = parent_.database(key[0]);
        auto result = db.retrieve(key);
        agg_.append(result);
    }

private: // members

    Retriever& parent_;
    HandleAggregator& agg_;
};

//----------------------------------------------------------------------------------------------------------------------

Retriever::Retriever(const Config& config, const Schema& schema, int lruSize) :
    config_(config),
    schema_(schema),
    databases_(lruSize) {}

api::Handle* Retriever::retrieve(const api::Query& query) {
    HandleAggregator agg;
    RetrieveVisitor visitor(*this, agg);
    schema_.walk(query, visitor);
    return agg.toHandle();
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