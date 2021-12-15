
#pragma once

#include <ostream>
#include <set>
#include <vector>
#include <map>
#include <array>
#include <unordered_map>
#include <unordered_set>


namespace dasi::util {
namespace internal {

//----------------------------------------------------------------------------------------------------------------------

template <typename L>
void print_list(std::ostream& s, const L& lst, const char* st="[", const char* end="]") {
    s << st;
    bool first = true;
    for (const auto& elem : lst) {
        if (!first) s << ", ";
        s << elem;
        first = false;
    }
    s << end;
}

template <typename M>
void print_map(std::ostream& s, const M& mp) {
    s << "{";
    bool first = true;
    for (const auto& elem : mp) {
        if (!first) s << ", ";
        s << elem.first << ":" << elem.second;
        first = false;
    }
    s << "}";
}

//----------------------------------------------------------------------------------------------------------------------

}
}

// Overrides go into namespace std for name lookup purposes (specialisations for
// templates in namespace std)

namespace std {

//----------------------------------------------------------------------------------------------------------------------

template <typename T>
inline std::ostream& operator<<(std::ostream& s, const std::vector<T>& v) {
    dasi::util::internal::print_list(s, v);
    return s;
}

template <typename T>
inline std::ostream& operator<<(std::ostream& s, const std::set<T>& v) {
    dasi::util::internal::print_list(s, v);
    return s;
}

template <typename T, std::size_t N>
inline std::ostream& operator<<(std::ostream& s, const std::array<T, N>& v) {
    dasi::util::internal::print_list(s, v);
    return s;
}

template <typename T>
inline std::ostream& operator<<(std::ostream& s, const std::unordered_set<T>& v) {
    dasi::util::internal::print_list(s, v);
    return s;
}

template <typename S, typename T, typename C, typename A>
inline std::ostream& operator<<(std::ostream& s, const std::map<S,T,C,A>& v) {
    dasi::util::internal::print_map(s, v);
    return s;
}

template <typename S, typename T, typename H, typename KE, typename A>
inline std::ostream& operator<<(std::ostream& s, const std::unordered_map<S,T,H,KE,A>& v) {
    dasi::util::internal::print_map(s, v);
    return s;
}

//----------------------------------------------------------------------------------------------------------------------

}