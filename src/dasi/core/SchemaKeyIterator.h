
#pragma once

#include "dasi/core/SplitReferenceKey.h"
#include "dasi/util/CartesianProduct.h"

namespace dasi {

//----------------------------------------------------------------------------------------------------------------------

// Given input from a schema, generate a list of keys

template <typename TContainer>
class SchemaKeyIterator {

public: // methods

    void insert(int level, const std::string& key, const TContainer& values);

    template <typename TVisitor>
    void visit(TVisitor& visitor);

private: // members

    SplitReferenceKey key_;
    CartesianProduct<TContainer, std::string_view> products_[3];
};

//----------------------------------------------------------------------------------------------------------------------

template <typename TContainer>
void SchemaKeyIterator<TContainer>::insert(int level, const std::string& key, const TContainer& values) {
    auto it = key_[level].set(key, "");
    products_[level].append(values, it->second);
}


namespace detail {

template<typename T, typename = void>
struct has_first_level_fn : std::false_type {};
template<typename T>
struct has_first_level_fn<T, std::void_t<decltype(std::declval<T>().firstLevel(std::declval<dasi::SplitReferenceKey>()))>> : std::true_type {};

template<typename T, typename = void>
struct has_second_level_fn : std::false_type {};
template<typename T>
struct has_second_level_fn<T, std::void_t<decltype(std::declval<T>().secondLevel(std::declval<dasi::SplitReferenceKey>()))>> : std::true_type {};

}

template <typename TContainer>
template <typename TVisitor>
void SchemaKeyIterator<TContainer>::visit(TVisitor& visitor) {

    // Iterate at all 3 levels. The CartesianProduct objects manipulate the
    // values referred to in the SplitReferenceKey.
    //
    // n.b. This is where the actual tensor product of all the values in a request
    //      is done -> we only actually have to walk the entire schema once.
    //      and we don't have to push/pop loads of values, and do lots of string
    //      manipulations.
    while (products_[0].next()) {
        if constexpr (detail::has_first_level_fn<TVisitor>::value) {
            visitor.firstLevel(key_);
        }
        while (products_[1].next()) {
            if constexpr (detail::has_second_level_fn<TVisitor>::value) {
                visitor.secondLevel(key_);
            }
            while (products_[2].next()) {
                visitor.thirdLevel(key_);
            }
        }
    }
}

//----------------------------------------------------------------------------------------------------------------------

}
