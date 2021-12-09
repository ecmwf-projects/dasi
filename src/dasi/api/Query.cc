
#include "dasi/api/Query.h"

#include "dasi/util/ContainerIostream.h"
#include "dasi/util/Exceptions.h"
#include "dasi/util/StringBuilder.h"

namespace dasi {

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

auto Query::get(const std::string_view& name) const -> const value_type& {
    auto it = values_.find(name);
    if (it == values_.end()) throw KeyError((StringBuilder() << name << " not found in Query").str(), Here());
    return it->second;
}

//----------------------------------------------------------------------------------------------------------------------

}