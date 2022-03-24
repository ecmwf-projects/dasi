
#include "dasi/core/HandleFactory.h"

#include "dasi/util/Exceptions.h"
#include "dasi/util/StringBuilder.h"

namespace dasi::core {

//----------------------------------------------------------------------------------------------------------------------

HandleBuilderBase::HandleBuilderBase(std::string name) :
    name_(name) {

    HandleFactory::instance().add(name_, this);
}

HandleBuilderBase::~HandleBuilderBase() {
    HandleFactory::instance().remove(name_);
}

//----------------------------------------------------------------------------------------------------------------------

HandleFactory::HandleFactory() {}

HandleFactory& HandleFactory::instance() {
    static HandleFactory factory;
    return factory;
}

void HandleFactory::add(const std::string& name, HandleBuilderBase* builder) {
    builders_[name] = builder;
}

void HandleFactory::remove(const std::string& name) {
    builders_.erase(name);
}

api::ReadHandle* HandleFactory::buildReadHandle(const std::string& name, const std::string& location,
    HandleBuilderBase::offset_type offset, HandleBuilderBase::length_type length) {

    auto it = builders_.find(name);

    if (it == builders_.end()) {
        std::string msg = util::StringBuilder() << "No handle builder found for type '" << name << "'";
        throw util::SeriousBug(msg, Here());
    }

    return it->second->makeReadHandle(location, offset, length);
}

//----------------------------------------------------------------------------------------------------------------------

}