
#include "dasi/api/Key.h"

#include "dasi/util/ContainerIostream.h"

namespace dasi {

//----------------------------------------------------------------------------------------------------------------------

Key::Key(std::initializer_list<std::pair<const std::string, std::string>> l) :
    values_(l) {}


void Key::print(std::ostream& s) const {
    s << values_;
}

bool Key::has(const std::string_view& name) const {
    return (values_.find(name) != values_.end());
}

void Key::set(const std::string& k, const std::string_view& v) {
    values_.insert_or_assign(k, v);
}

//----------------------------------------------------------------------------------------------------------------------

}