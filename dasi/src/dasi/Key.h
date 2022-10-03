
/// @author Simon Smart
/// @date   Sep 2022

#pragma once

#include <initializer_list>
#include <map>
#include <string>
#include <utility>

namespace dasi {

//-------------------------------------------------------------------------------------------------

/// @note - This Key class can be made into a thinner wrapper of the FDB Key class, if required.
///         But it is currently written to be separable from the FDB.

class Key {

    using value_type = std::string;

    /// @note use of transparent comparator --> allow lookup with std::string_view as key
    using map_type = std::map<std::string, value_type, std::less<>>;

public: // methods

    Key() = default;
    Key(std::initializer_list<std::pair<const std::string, std::string>>);

    /** Is there a value corresponding to the specified key?
     * @param name The key to look up the corresponding value
     */
    [[nodiscard]]
    bool has(const char* name) const;
    [[nodiscard]]
    bool has(const std::string_view& name) const;
    [[nodiscard]]
    bool has(const std::string& name) const;

    /** Set the value corresponding to a specified key
     * @param k The key to set the value of
     * @param v The value to set
     * @return An iterator to the pair<> corresponding to the just set/updated value
     */
    typename map_type::iterator set(const std::string& k, const value_type& v);

    /** Constant iterator accessors to the key:value pairs stored */
    typename map_type::const_iterator begin() const;
    typename map_type::const_iterator end() const;

    /** Return the number of key:value pairs stored */
    typename map_type::size_type size() const;

    /** Erase all key:value pairs stored */
    void clear();

private: // methods

    void print(std::ostream& s) const;

private: // friends

    friend std::ostream& operator<<(std::ostream& s, const Key& k) {
        k.print(s);
        return s;
    }

private: // members

    map_type values_;
};

//-------------------------------------------------------------------------------------------------

} // namespace dasi


