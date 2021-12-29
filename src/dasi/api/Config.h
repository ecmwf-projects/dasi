
#pragma once

#include <memory>
#include <string>
#include <iosfwd>


namespace YAML {
class Node;
}

namespace dasi::api {

//----------------------------------------------------------------------------------------------------------------------

class Config {
public: // methods

    Config();
    explicit Config(const char* cfg);
    explicit Config(std::istream& in);
    explicit Config(const YAML::Node& cfg);
    ~Config();

    /// @note - if we need to add nested/sub configurations, this is the place to do the subsetting.
    ///         Remember to distinguish between config that is global, and that which nests.

    /// @note - YAML does not support indexing with string_views :-(

    [[ nodiscard ]] bool has(const char* name) const;
    [[ nodiscard ]] bool has(const std::string& name) const;

    // n.b. this is not very nice. Returns a copy not a reference...
    [[ nodiscard ]] YAML::Node value(const char* name) const;
    [[ nodiscard ]] YAML::Node value(const std::string& name) const;

    // n.b. not const std::string&. We could do this better, if this becomes an issue.
    [[ nodiscard ]] std::string getString(const char* name) const;
    [[ nodiscard ]] std::string getString(const std::string& name) const;

    [[ nodiscard ]] long getLong(const char* name) const;
    [[ nodiscard ]] long getLong(const std::string& name) const;

    [[ nodiscard ]] long getLong(const char* name, long defVal) const;
    [[ nodiscard ]] long getLong(const std::string& name, long defVal) const;

private: // members

    std::unique_ptr<YAML::Node> values_;
};

//----------------------------------------------------------------------------------------------------------------------

}