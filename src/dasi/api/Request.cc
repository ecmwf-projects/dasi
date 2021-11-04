
#include "dasi/api/Request.h"

#include "dasi/core/ContainerIostream.h"

namespace dasi {

//----------------------------------------------------------------------------------------------------------------------

Request::Request(std::initializer_list<std::pair<const std::string, std::vector<std::string>>> l) :
    values_(l) {}


void Request::print(std::ostream& s) const {
    s << values_;
}

bool Request::has(const std::string_view& name) const {
    return (values_.find(name) != values_.end());
}

void Request::set(const std::string& k, std::initializer_list<std::string> v) {
    values_.insert_or_assign(k, v);
}

//----------------------------------------------------------------------------------------------------------------------

}