
#pragma once

#include <vector>
#include <string>
#include <iosfwd>
#include <initializer_list>

#include <iostream>

#include "dasi/api/Key.h"
#include "dasi/api/SplitKey.h"

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

    template <typename TVisitor>
    void walk(const Key& key, TVisitor& visitor) const;

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
class SchemaRuleParent : public SchemaRule<TSelf> {

public: // methods

    SchemaRuleParent(std::initializer_list<std::string> keys,
                     std::initializer_list<ChildRule> subrules);
    SchemaRuleParent(const YAML::Node& yml);

    void print(std::ostream& s) const override;

    template <typename TVisitor>
    void walkChildren(const Key& key, TVisitor& visitor) const;

private: // members

    std::vector<ChildRule> children_;
};

//----------------------------------------------------------------------------------------------------------------------

class SchemaRule3 : public SchemaRule<SchemaRule3> {
public:
    using SchemaRule<SchemaRule3>::SchemaRule;

    template <typename TVisitor>
    void walkMatched(const Key& key, TVisitor& visitor) const;
};

class SchemaRule2 : public SchemaRuleParent<SchemaRule2, SchemaRule3> {
public:
    using SchemaRuleParent<SchemaRule2, SchemaRule3>::SchemaRuleParent;

    template <typename TVisitor>
    void walkMatched(const Key& key, TVisitor& visitor) const;
};

class SchemaRule1 : public SchemaRuleParent<SchemaRule1, SchemaRule2> {
public:
    using SchemaRuleParent<SchemaRule1, SchemaRule2>::SchemaRuleParent;

    template <typename TVisitor>
    void walkMatched(const Key& key, TVisitor& visitor) const;
};

//----------------------------------------------------------------------------------------------------------------------

class Schema {

public: // methods

    Schema(std::initializer_list<SchemaRule1> rules);
    Schema(std::vector<SchemaRule1>&& rules);

    static Schema parse(std::istream& s);

    template <typename TVisitor>
    void walk(const Key& key, TVisitor& visitor);

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

template <typename TVisitor>
void Schema::walk(const Key& key, TVisitor& visitor) {
//    SplitKey matched;
    for (const auto& rule : rules_) {
//        rule.walk(key, matched, visitor);
        rule.walk(key, visitor);
    }
}

template <typename TSelf>
template <typename TVisitor>
void SchemaRule<TSelf>::walk(const Key& key, TVisitor& visitor) const {
    for (const auto& k : keys_) {
        if (!key.has(k)) {
            return;
        }
    }

    static_cast<const TSelf&>(*this).walkMatched(key, visitor);
}

template <typename TSelf, typename ChildRule>
template <typename TVisitor>
void SchemaRuleParent<TSelf, ChildRule>::walkChildren(const Key& key, TVisitor& visitor) const {
    for (const auto& child : children_) {
        child.walk(key, visitor);
    }
}

template <typename TVisitor>
void SchemaRule1::walkMatched(const Key& key, TVisitor& visitor) const {
    std::cout << "Matched level 1" << std::endl;
    walkChildren(key, visitor);
}

template <typename TVisitor>
void SchemaRule2::walkMatched(const Key& key, TVisitor& visitor) const {
    std::cout << "Matched level 2" << std::endl;
    walkChildren(key, visitor);
}

template <typename TVisitor>
void SchemaRule3::walkMatched(const Key& key, TVisitor& visitor) const {
    std::cout << "Matched level 3" << std::endl;
}


//----------------------------------------------------------------------------------------------------------------------

}