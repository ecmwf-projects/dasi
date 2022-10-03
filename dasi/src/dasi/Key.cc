
#include "dasi/Key.h"

#include "eckit/types/Types.h"


namespace dasi {

//-------------------------------------------------------------------------------------------------

Key::Key(std::initializer_list<std::pair<const std::string, std::string>> l) :
    values_(l) {
}

void Key::print(std::ostream& s) const {
    s << values_;
}

bool Key::has(const char* name) const {
    return has(std::string_view{name});
}

bool Key::has(const std::string_view& name) const {
    return values_.find(name) != values_.end();
}

bool Key::has(const std::string& name) const {
    return values_.find(name) != values_.end();
}

Key::map_type::iterator Key::set(const std::string& k, const value_type& v) {
    return values_.insert_or_assign(k, v).first;
}

Key::map_type::const_iterator Key::begin() const {
    return values_.begin();
}

Key::map_type::const_iterator Key::end() const {
    return values_.end();
}

Key::map_type::size_type Key::size() const {
    return values_.size();
}

void Key::clear() {
    values_.clear();
}

//-------------------------------------------------------------------------------------------------

} // namespace dasi
