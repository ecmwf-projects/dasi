
#pragma once

#include <string>
#include <initializer_list>
#include <iosfwd>
#include <array>
#include <algorithm>

#include "dasi/core/OrderedReferenceKey.h"
#include "dasi/util/ContainerIostream.h"

namespace dasi::core {

//----------------------------------------------------------------------------------------------------------------------

template <typename KEY>
class SplitKeyT {

public: // method

    SplitKeyT() = default;
    SplitKeyT(std::initializer_list<std::pair<const std::string, typename KEY::value_type>> init0,
              std::initializer_list<std::pair<const std::string, typename KEY::value_type>> init1,
              std::initializer_list<std::pair<const std::string, typename KEY::value_type>> init2) :
        keys_ {init0, init1, init2} {
    }

    KEY& operator[](int level) { return keys_[level]; }
    const KEY& operator[](int level) const { return keys_[level]; }

    bool operator==(const SplitKeyT& rhs) { return keys_ == rhs.keys_; }

    template<typename KEY2>
    bool operator==(const SplitKeyT<KEY2>& rhs) const { return std::equal(keys_.begin(), keys_.end(), rhs.keys_.begin()); }

    template<typename KEY2 = KEY>
    KEY2 join() const {
        KEY2 joined;
        for (const auto& key : keys_) {
            for (const auto &kv : key) {
                joined.set(kv.first, kv.second);
            }
        }
        return joined;
    }

private: // methods

    void print(std::ostream& s) const { s << keys_; }

private: // friends

    friend std::ostream& operator<<(std::ostream& s, const SplitKeyT& k) {
        k.print(s);
        return s;
    }

    template<typename KEY2>
    friend class SplitKeyT;

protected: // members

    std::array<KEY, 3> keys_;
};

//----------------------------------------------------------------------------------------------------------------------

}