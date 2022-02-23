
#include "dasi/api/Handle.h"
#include "dasi/api/Query.h"
#include "dasi/api/Result.h"

#include "dasi/core/Retriever.h"

using dasi::api::Config;

namespace dasi::core {

//----------------------------------------------------------------------------------------------------------------------

class ResultAggregator {

public: // methods

    ~ResultAggregator() {
        for(auto handle : handles_) {
            delete handle;
        }
    }

    void append(const api::Key& key, api::Handle* handle) {
        keys_.push_back(key);
        handles_.push_back(handle);
    }

    api::Result toResult() {
        return api::Result{std::move(keys_), std::move(handles_)};
    }

private: // members

    std::vector<api::Key> keys_;
    std::vector<api::Handle*> handles_;
};

//----------------------------------------------------------------------------------------------------------------------

class RetrieveVisitor {

public: // methods

    RetrieveVisitor(Retriever& parent, ResultAggregator& agg) :
        parent_(parent),
        agg_(agg) {}

    void thirdLevel(SplitReferenceKey& key) {
        std::cout << "Third level in retrieve!!!" << std::endl;
        DB& db = parent_.database(key[0]);
        auto result = db.retrieve(key);
        api::Key key2;
        for (int level = 0; level < 3; ++level) {
            for (const auto& elem : key[level]) {
                key2.set(elem.first, std::string{elem.second});
            }
        }
        agg_.append(key2, result);
    }

private: // members

    Retriever& parent_;
    ResultAggregator& agg_;
};

//----------------------------------------------------------------------------------------------------------------------

Retriever::Retriever(const Config& config, const Schema& schema, int lruSize) :
    config_(config),
    schema_(schema),
    databases_(lruSize) {}

api::Result Retriever::retrieve(const api::Query& query) {
    ResultAggregator agg;
    RetrieveVisitor visitor(*this, agg);
    schema_.walk(query, visitor);
    return agg.toResult();
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