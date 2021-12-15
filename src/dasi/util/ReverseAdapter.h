
namespace dasi::util {

//----------------------------------------------------------------------------------------------------------------------

/// An adapter to iterate a container in reverse (using rbegin/rend)

template <typename T>
class ReverseAdapter {

    T& container_;

public: // types

    using value_type = typename T::value_type;

    using iterator = typename T::reverse_iterator;
    using const_iterator = typename T::const_reverse_iterator;

    using reverse_iterator = typename T::iterator;
    using const_reverse_iterator = typename T::const_iterator;

public: // methods

    ReverseAdapter(T& container) :
        container_(container) {}

    // We remove the copy and move constructors, otherwise ReverseAdapter(ReverseAdapter(container))
    // reduces itself to ReverseAdapter(container) silently, which is a bit nasty!
    ReverseAdapter(const ReverseAdapter<T>& rhs) = delete;
    ReverseAdapter(ReverseAdapter<T>&& rhs) = delete;

    iterator begin() { return container_.rbegin(); }
    iterator end() { return container_.rend(); }

    const_iterator begin() const { return container_.crbegin(); }
    const_iterator end() const { return container_.crend(); }

    const_iterator cbegin() const { return container_.crbegin(); }
    const_iterator cend() const { return container_.crend(); }

    reverse_iterator rbegin() { return container_.begin(); }
    reverse_iterator rend() { return container_.end(); }

    const_reverse_iterator rbegin() const { return container_.cbegin(); }
    const_reverse_iterator rend() const { return container_.cend(); }

    const_reverse_iterator crbegin() const { return container_.cbegin(); }
    const_reverse_iterator crend() const { return container_.cend(); }
};

//----------------------------------------------------------------------------------------------------------------------

} // namespace dasi
