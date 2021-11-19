#pragma once

#include "dasi/util/Exceptions.h"

namespace dasi {

//----------------------------------------------------------------------------------------------------------------------

template <typename TContainer>
class CartesianProduct {

public: // types

    using value_type = typename TContainer::value_type;

public: // methods

    CartesianProduct() = default;
    ~CartesianProduct() = default;

    void append(const TContainer& input, value_type& output);
    void append(const value_type& input, value_type& output);

    bool next();

private: // members

    std::vector<std::reference_wrapper<const TContainer>> inputs_;
    std::vector<std::reference_wrapper<value_type>> output_;

    std::vector<size_t> indices_;
};

//----------------------------------------------------------------------------------------------------------------------

//template <typename TContainer, typename TOutput>
//CartesianProduct<TContainer, TOutput>::CartesianProduct(TOutput& output) : output_(output) {}

template <typename TContainer>
void CartesianProduct<TContainer>::append(const TContainer& input, value_type& output) {
    inputs_.push_back(std::cref(input));
    output_.push_back(std::ref(output));
}

template <typename TContainer>
void CartesianProduct<TContainer>::append(const value_type& input, value_type& output) {
    output = input;
}

template <typename TContainer>
bool CartesianProduct<TContainer>::next() {

    // If this is the first iteration, initialise

    if (indices_.empty()) {
        ASSERT(!inputs_.empty());
        ASSERT(inputs_.size() == output_.size());
        indices_.resize(inputs_.size(), 0);
        for (int depth = 0; depth < inputs_.size(); ++depth) {
            output_[depth].get() = inputs_[depth].get()[0];
        }
        return true;
    }

    int depth;
    for (depth = 0; depth < inputs_.size(); ++depth) {
        indices_[depth]++;
        if (indices_[depth] < inputs_[depth].get().size()) {
            output_[depth].get() = inputs_[depth].get()[indices_[depth]];
            return true;
        } else {
            indices_[depth] = 0;
            output_[depth].get() = inputs_[depth].get()[indices_[depth]];
        }
    }

    indices_.clear();
    return false;
}

//----------------------------------------------------------------------------------------------------------------------

}
