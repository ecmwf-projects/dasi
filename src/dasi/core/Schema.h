
#pragma once

#include <vector>
#include <string>
#include <iosfwd>
#include <initializer_list>

#include <iostream>

#include "dasi/api/Key.h"
#include "dasi/core/SchemaKeyIterator.h"

namespace YAML {
class Node;
}

namespace dasi::core {

//----------------------------------------------------------------------------------------------------------------------

template <typename Tself, int LEVEL>
class SchemaRule {

public: // methods

    SchemaRule() = default;
    SchemaRule(std::initializer_list<std::string>);
    explicit SchemaRule(const YAML::Node& yml);

    virtual void print(std::ostream& s) const;

    template <typename TRequest, typename TVisitor>
    void walk(const TRequest& key,
              SchemaKeyIterator<typename TRequest::value_type>& matched,
              TVisitor& visitor) const;

protected: // members

    std::vector<std::string> keys_;
};

template <typename T, int L>
inline std::ostream& operator<<(std::ostream& s, const SchemaRule<T, L>& r) {
    r.print(s);
    return s;
}

//----------------------------------------------------------------------------------------------------------------------

template <typename TSelf, typename ChildRule, int LEVEL>
class SchemaRuleParent : public SchemaRule<TSelf, LEVEL> {

public: // methods

    SchemaRuleParent(std::initializer_list<std::string> keys,
                     std::initializer_list<ChildRule> subrules);
    explicit SchemaRuleParent(const YAML::Node& yml);

    void print(std::ostream& s) const override;

    template <typename TRequest, typename TVisitor>
    void walkMatched(const TRequest& key,
                     SchemaKeyIterator<typename TRequest::value_type>& matched,
                     TVisitor& visitor) const;

private: // members

    std::vector<ChildRule> children_;
};

//----------------------------------------------------------------------------------------------------------------------

class SchemaRule3 : public SchemaRule<SchemaRule3, 2> {
public:
    using SchemaRule<SchemaRule3, 2>::SchemaRule;

    template <typename TRequest, typename TVisitor>
    void walkMatched(const TRequest& key,
                     SchemaKeyIterator<typename TRequest::value_type>& matched,
                     TVisitor& visitor) const;
};

class SchemaRule2 : public SchemaRuleParent<SchemaRule2, SchemaRule3, 1> {
public:
    using SchemaRuleParent<SchemaRule2, SchemaRule3, 1>::SchemaRuleParent;
};

class SchemaRule1 : public SchemaRuleParent<SchemaRule1, SchemaRule2, 0> {
public:
    using SchemaRuleParent<SchemaRule1, SchemaRule2, 0>::SchemaRuleParent;
};

//----------------------------------------------------------------------------------------------------------------------

class Schema {

public: // methods

    Schema(std::initializer_list<SchemaRule1> rules);
    explicit Schema(std::vector<SchemaRule1>&& rules);
    explicit Schema(const YAML::Node& config);
    explicit Schema(const std::vector<YAML::Node>& config);

    static Schema parse(std::istream& s);
    static Schema parse(const char* s);

    template <typename TRequest, typename TVisitor>
    void walk(const TRequest& key, TVisitor& visitor) const;

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

template <typename TRequest, typename TVisitor>
void Schema::walk(const TRequest& key, TVisitor& visitor) const {
    SchemaKeyIterator<typename TRequest::value_type> matched;
    for (const auto& rule : rules_) {
        rule.walk(key, matched, visitor);
    }
}

template <typename TSelf, int LEVEL>
template <typename TRequest, typename TVisitor>
void SchemaRule<TSelf, LEVEL>::walk(const TRequest& key,
                             SchemaKeyIterator<typename TRequest::value_type>& matched,
                             TVisitor& visitor) const {
    for (const auto& k : keys_) {
        if (!key.has(k)) {
            return;
        }
    }

    for (const auto& k : keys_) {
        matched.insert(LEVEL, k, key.get(k));
    }

    static_cast<const TSelf&>(*this).walkMatched(key, matched, visitor);
}

template <typename TSelf, typename ChildRule, int LEVEL>
template <typename TRequest, typename TVisitor>
void SchemaRuleParent<TSelf, ChildRule, LEVEL>::walkMatched(const TRequest& key,
                                                            SchemaKeyIterator<typename TRequest::value_type>& matched,
                                                            TVisitor& visitor) const {
    for (const auto& child : children_) {
        child.walk(key, matched, visitor);
    }
}

// This is built with concepts. We could do something SFINAE if necessary (i.e. c++17)

//template <typename T>
//concept reportFirstLevel = requires (T t) {
//    { t.firstLevel() } -> std::same_as<void>;
//};


template <typename TRequest, typename TVisitor>
void SchemaRule3::walkMatched(const TRequest& key,
                              SchemaKeyIterator<typename TRequest::value_type>& matched,
                              TVisitor& visitor) const {

    matched.visit(visitor);
}


//----------------------------------------------------------------------------------------------------------------------

}