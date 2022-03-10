
#pragma once

#include <condition_variable>
#include <exception>
#include <mutex>
#include <optional>
#include <queue>
#include <thread>

#include "dasi/util/Generator.h"

namespace dasi::util {

//----------------------------------------------------------------------------------------------------------------------

template<typename T>
class AsyncGenerator : public GeneratorBase<T> {

public: // methods

    AsyncGenerator(size_t maxQueue);

    template<typename F, typename... Args>
    AsyncGenerator(size_t maxQueue, F f, Args&&... args);

    ~AsyncGenerator();

    void push(const T&) override;
    void push(T&&) override;
    void interrupt(std::exception_ptr) override;
    void finish() override;
    bool next() override;
    T& get() override;

private: // methods

    bool checkInterrupt(bool checkFinish = false);

private: // members

    size_t max_;

    bool finished_;
    std::exception_ptr interrupt_;
    std::optional<T> current_;
    std::queue<T> queue_;

    std::condition_variable cv_;
    std::mutex mutex_;
    std::thread thr_;

};

//----------------------------------------------------------------------------------------------------------------------

namespace detail {

class StopGenerator : public std::exception {};

}

//----------------------------------------------------------------------------------------------------------------------

template<typename T>
AsyncGenerator<T>::AsyncGenerator(size_t maxQueue) :
    max_(maxQueue), finished_(false) {}

template<typename T>
template<typename F, typename... Args>
AsyncGenerator<T>::AsyncGenerator(size_t maxQueue, F f, Args&&... args) :
    AsyncGenerator(maxQueue) {
    
    thr_ = std::thread([this, f, args...] {
        try {
            f(*this, std::forward<Args>(args)...);
            finish();
        }
        catch (detail::StopGenerator) {
        }
        catch (...) {
            interrupt(std::current_exception());
        }
    });
}

template<typename T>
AsyncGenerator<T>::~AsyncGenerator() {
    finish();
    if (thr_.joinable()) {
        thr_.join();
    }
}

template<typename T>
void AsyncGenerator<T>::push(const T& elem) {
    std::unique_lock<std::mutex> lock(mutex_);
    cv_.wait(lock, [this] { return checkInterrupt(true) && (queue_.size() < max_); });
    queue_.push(elem);
    cv_.notify_all();
}

template<typename T>
void AsyncGenerator<T>::push(T&& elem) {
    std::unique_lock<std::mutex> lock(mutex_);
    cv_.wait(lock, [this] { return checkInterrupt(true) && (queue_.size() < max_); });
    queue_.emplace(std::move(elem));
    cv_.notify_all();
}

template<typename T>
void AsyncGenerator<T>::interrupt(std::exception_ptr exc) {
    std::lock_guard<std::mutex> lock(mutex_);
    interrupt_ = exc;
    cv_.notify_all();
}

template<typename T>
bool AsyncGenerator<T>::checkInterrupt(bool checkFinish) {
    if (interrupt_)  {
        std::rethrow_exception(interrupt_);
    }
    if (checkFinish && finished_) {
        throw detail::StopGenerator();
    }
    return true;
}

template<typename T>
void AsyncGenerator<T>::finish() {
    std::lock_guard<std::mutex> lock(mutex_);
    finished_ = true;
    cv_.notify_all();
}

template<typename T>
bool AsyncGenerator<T>::next() {
    std::unique_lock<std::mutex> lock(mutex_);
    cv_.wait(lock, [this] { return checkInterrupt() && (finished_ || !queue_.empty()); });
    if (finished_ && queue_.empty()) {
        current_.reset();
        return false;
    }
    ASSERT(!queue_.empty());
    current_ = queue_.front();
    queue_.pop();
    cv_.notify_all();
    return true;
}

template<typename T>
T& AsyncGenerator<T>::get() {
    // lock only needed if we allow next() and get() to be called from different threads
    std::lock_guard<std::mutex> lock(mutex_);
    checkInterrupt();
    ASSERT(current_);
    return *current_;
}

//----------------------------------------------------------------------------------------------------------------------

}