
#pragma once

#include <vector>
#include <string>
#include <iosfwd>
#include <initializer_list>

namespace YAML {
class Node;
}

namespace dasi {

class Key;

//----------------------------------------------------------------------------------------------------------------------

template <typename Tself>
class SchemaRule {

public: // methods

    SchemaRule() = default;
    SchemaRule(std::initializer_list<std::string>);
    SchemaRule(const YAML::Node& yml);

    virtual void print(std::ostream& s) const;

    void walk(const Key& key) const;

protected: // members

    std::vector<std::string> keys_;
};

template <typename T>
inline std::ostream& operator<<(std::ostream& s, const SchemaRule<T>& r) {
    r.print(s);
    return s;
}

//----------------------------------------------------------------------------------------------------------------------

template <typename TSelf, typename ChildRule>
class SchemaRuleParent : public SchemaRule<SchemaRuleParent<TSelf, ChildRule>> {

public: // methods

    SchemaRuleParent(std::initializer_list<std::string> keys,
                     std::initializer_list<ChildRule> subrules);
    SchemaRuleParent(const YAML::Node& yml);

    void print(std::ostream& s) const override;

    void walkChildren(const Key& key) const;

private: // members

    std::vector<ChildRule> children_;
};

//----------------------------------------------------------------------------------------------------------------------

class SchemaRule3 : public SchemaRule<SchemaRule3> {
public:
    using SchemaRule::SchemaRule;
    void walkChildren(const Key& key) const {}
};

class SchemaRule2 : public SchemaRuleParent<SchemaRule2, SchemaRule3> {
public:
    using SchemaRuleParent::SchemaRuleParent;
};

class SchemaRule1 : public SchemaRuleParent<SchemaRule1, SchemaRule2> {
public:
    using SchemaRuleParent::SchemaRuleParent;
};

//----------------------------------------------------------------------------------------------------------------------

class Schema {

public: // methods

    Schema(std::initializer_list<SchemaRule1> rules);
    Schema(std::vector<SchemaRule1>&& rules);

    static Schema parse(std::istream& s);

    void walk(const Key& key);

private: // methods

    void print(std::ostream& o) const;

    friend std::ostream& operator<<(std::ostream& o, const Schema& s) {
        s.print(o);
        return o;
    }

private: // members

    std::vector<SchemaRule1> rules_;
};

//----------------------------------------------------------------------------------------------------------------------

}