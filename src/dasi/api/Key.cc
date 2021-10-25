
#include "dasi/api/Key.h"

#include "dasi/core/ContainerIostream.h"

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

//----------------------------------------------------------------------------------------------------------------------

}