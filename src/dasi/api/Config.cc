
#include "dasi/api/Config.h"
#include "dasi/util/Exceptions.h"

#include "yaml-cpp/yaml.h"


namespace dasi::api {

namespace {

template <typename Key>
bool yamlHas(const YAML::Node& node, const Key& k) {
    return node[k].IsDefined();
}

template <typename T, typename Key>
T castedValue(const YAML::Node& node, const Key& key) {
    YAML::Node val = node[key];
    if (val.IsDefined()) {
        try {
            return val.as<T>();
        } catch (YAML::BadConversion& e) {
            std::ostringstream ss;
            ss << "Could not convert node '" << key << "' to " << typeid(T).name();
            throw util::InvalidConfiguration(ss.str(), Here());
        }
    } else {
        std::ostringstream ss;
        ss << "Key '" << key << "' does not exist";
        throw util::InvalidConfiguration(ss.str(), Here());
    }
}

template <typename T, typename Key, typename S>
T castedValueDefault(const YAML::Node& node, const Key& key, const S& defval) {
    YAML::Node val = node[key];
    if (val.IsDefined()) {
        try {
            return val.as<T>();
        } catch (YAML::BadConversion& e) {
            std::ostringstream ss;
            ss << "Could not convert node '" << key << "' to " << typeid(T).name();
            throw util::InvalidConfiguration(ss.str(), Here());
        }
    } else {
        return defval;
    }
}

YAML::Node parseCharStar(const char* config) {
    std::istringstream iss(config);
    return YAML::Load(iss);
}

}

//----------------------------------------------------------------------------------------------------------------------

Config::Config() :
    values_(new YAML::Node) {}

Config::Config(const char* cfg) :
    values_(new YAML::Node(parseCharStar(cfg))) {}

Config::Config(std::istream& in) :
    Config(YAML::Load(in)) {}

Config::Config(const YAML::Node& config) :
    values_(new YAML::Node{config}) {}

Config::~Config() {}

bool Config::has(const char* name) const {
    return yamlHas(*values_, name);
}

bool Config::has(const std::string& name) const {
    return yamlHas(*values_, name);
}

YAML::Node Config::value(const char* name) const {
    return (*values_)[name];
}

YAML::Node Config::value(const std::string& name) const {
    return (*values_)[name];
}

std::string Config::getString(const char* name) const {
    return castedValue<std::string>(*values_, name);
}

std::string Config::getString(const std::string& name) const {
    return castedValue<std::string>(*values_, name);
}

long Config::getLong(const char* name) const {
    return castedValue<long>(*values_, name);
}

long Config::getLong(const std::string& name) const {
    return castedValue<long>(*values_, name);
}

long Config::getLong(const char* name, long defVal) const {
    return castedValueDefault<long>(*values_, name, defVal);
}

long Config::getLong(const std::string& name, long defVal) const {
    return castedValueDefault<long>(*values_, name, defVal);
}

//----------------------------------------------------------------------------------------------------------------------

}