
#include "dasi/api/Query.h"

#include "dasi/util/ContainerIostream.h"
#include "dasi/util/Exceptions.h"
#include "dasi/util/StringBuilder.h"

namespace dasi::api {

//----------------------------------------------------------------------------------------------------------------------

Query::Query(std::initializer_list<std::pair<const std::string, std::vector<std::string>>> l) :
    values_(l) {}


void Query::print(std::ostream& s) const {
    s << values_;
}

bool Query::has(const std::string_view& name) const {
    return (values_.find(name) != values_.end());
}

void Query::set(const std::string& k, std::initializer_list<std::string> v) {
    values_.insert_or_assign(k, v);
}

void Query::set(const std::string& k, const value_type& v) {
    values_.insert_or_assign(k, v);
}

void Query::append(const std::string& k, const std::string& v) {
    auto it = values_.find(k);
    if (it == values_.end()) {
        set(k, {v});
        return;
    }
    values_[k].push_back(v);
}


auto Query::get(const std::string_view& name) const -> const value_type& {
    using util::StringBuilder;
    using util::KeyError;
    auto it = values_.find(name);
    if (it == values_.end()) throw KeyError((StringBuilder() << name << " not found in Query").str(), Here());
    return it->second;
}

//----------------------------------------------------------------------------------------------------------------------

}
