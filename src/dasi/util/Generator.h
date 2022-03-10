
#pragma once

#include <exception>
#include <memory>

#include "dasi/util/Exceptions.h"

namespace dasi::util {

//----------------------------------------------------------------------------------------------------------------------

template<typename T>
class GeneratorBase {

public: // methods

    virtual ~GeneratorBase() {}

    virtual void push(const T&) = 0;
    virtual void push(T&&) = 0;
    virtual void interrupt(std::exception_ptr) = 0;
    virtual void finish() = 0;
    virtual bool next() = 0;
    virtual T& get() = 0;

};

//----------------------------------------------------------------------------------------------------------------------

template<typename T>
class Generator {

public: // types

    using value_type = T;

    class Iter;

public: // methods

    Generator(GeneratorBase<T>* gen) : gen_(gen) {}

    Iter begin() { return Iter(gen_); }
    Iter end() { return Iter{}; }

private: // members

    std::shared_ptr<GeneratorBase<T>> gen_;

public: // classes

    class Iter {
    
    public: // types

        using value_type = Generator::value_type;
        using reference = value_type&;

    public: // methods

        Iter() : valid_(false), gen_(nullptr) {}
        Iter(const std::shared_ptr<GeneratorBase<T>>& gen) : valid_(true), gen_(gen) {
            valid_ = gen->next();
        }

        void operator++() {
            if (valid_) {
                valid_ = gen_->next();
            }
        }

        reference operator*() const {
            ASSERT(valid_);
            return gen_->get();
        }

        bool operator==(const Iter& rhs) const {
            return !(valid_ || rhs.valid_);
        }

        bool operator!=(const Iter& rhs) const {
            return valid_ || rhs.valid_;
        }

    private: // members

        bool valid_;
        std::shared_ptr<GeneratorBase<T>> gen_;
    };

};

//----------------------------------------------------------------------------------------------------------------------

}