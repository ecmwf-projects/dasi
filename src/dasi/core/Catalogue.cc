
#include "dasi/core/Catalogue.h"

#include "dasi/util/Exceptions.h"

#include <sstream>
#include <iostream>


namespace dasi::core {

//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------

CatalogueBuilderBase::CatalogueBuilderBase(const char* name) :
    name_(name) {
    CatalogueFactory::instance().enregister(name, this);
}

CatalogueBuilderBase::~CatalogueBuilderBase() {
    CatalogueFactory::instance().deregister(name_);
}

//----------------------------------------------------------------------------------------------------------------------

CatalogueFactory& CatalogueFactory::instance() {
    static CatalogueFactory theinstance;
    return theinstance;
}

void CatalogueFactory::enregister(const char* name, const CatalogueBuilderBase* builder) {
    std::lock_guard<std::mutex> lock(m_);
    ASSERT(builders_.emplace(std::make_pair(std::string(name), builder)).second);
}

void CatalogueFactory::deregister(const char* name) {
    std::lock_guard<std::mutex> lock(m_);
    auto it = builders_.find(name);
    ASSERT(it != builders_.end());
    builders_.erase(it);
}

Catalogue* CatalogueFactory::build(const std::string& name) {
    std::lock_guard<std::mutex> lock(m_);
    auto it = builders_.find(name);
    if (it == builders_.end()) {
        std::ostringstream ss;
        ss << "Unknown catalogue builder '" << name << "' not known";
        throw util::SeriousBug(ss.str(), Here());
    }
    return it->second->make();
}

void CatalogueFactory::list(std::ostream& s, const char* sep) {
    std::lock_guard<std::mutex> lock(m_);
    bool first = true;
    for (const auto& kv : builders_) {
        if (!first) s << sep;
        s << kv.first;
        first = false;
    }
}

//----------------------------------------------------------------------------------------------------------------------

}