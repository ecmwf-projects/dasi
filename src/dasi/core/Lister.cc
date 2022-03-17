
#include "dasi/api/Query.h"
#include "dasi/api/ListResult.h"

#include "dasi/core/Lister.h"

#include "dasi/util/AsyncGenerator.h"

using dasi::api::Config;

namespace dasi::core {

//----------------------------------------------------------------------------------------------------------------------

class ListVisitor {

public: // methods

    ListVisitor(Lister& parent, util::GeneratorBase<api::Key>& gen) :
        parent_(parent),
        gen_(gen) {}

    void thirdLevel(SplitReferenceKey& key) {
        DB& db = parent_.database(key[0]);
        if (!db.exists(key)) {
            return;
        }
        api::Key key2;
        for (int level = 0; level < 3; ++level) {
            for (const auto& elem : key[level]) {
                key2.set(elem.first, std::string{elem.second});
            }
        }
        gen_.push(std::move(key2));
    }

private: // members

    Lister& parent_;
    util::GeneratorBase<api::Key>& gen_;
};

//----------------------------------------------------------------------------------------------------------------------

Lister::Lister(const Config& config, const Schema& schema, int queueSize, int lruSize) :
    config_(config),
    schema_(schema),
    queueSize_(queueSize),
    databases_(lruSize) {}

api::ListResult Lister::list(const api::Query& query) {
    util::Generator<api::Key> keys(new util::AsyncGenerator<api::Key>(queueSize_, [this, query] (auto& gen) {
        ListVisitor visitor(*this, gen);
        schema_.walk(query, visitor);
    }));
    return api::ListResult(keys);
}

DB& Lister::database(const OrderedReferenceKey& dbkey) {
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