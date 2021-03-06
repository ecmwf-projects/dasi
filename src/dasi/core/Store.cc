
#include "dasi/core/Schema.h"

#include "dasi/util/ContainerIostream.h"
#include "dasi/util/IndentStream.h"
#include "dasi/util/Exceptions.h"

#include "yaml-cpp/yaml.h"

#include <istream>
#include <iostream>
#include <sstream>

namespace dasi::core {

//----------------------------------------------------------------------------------------------------------------------

template <typename TSelf>
SchemaRule<TSelf>::SchemaRule(std::initializer_list<std::string> l) :
    keys_(l) {}

template <typename TSelf>
SchemaRule<TSelf>::SchemaRule(const YAML::Node& yml) {

    if (!yml.IsSequence()) {
        throw InvalidConfiguration("Sequence not found in YAML schema", Here());
    }

    for (const auto& elem : yml) {
        keys_.emplace_back(elem.as<std::string>());
    }
}


template <typename TSelf>
void SchemaRule<TSelf>::print(std::ostream& s) const {
    s << keys_;
}

template <typename TSelf, typename ChildRule>
SchemaRuleParent<TSelf, ChildRule>::SchemaRuleParent(std::initializer_list<std::string> keys, std::initializer_list<ChildRule> subrules) :
    SchemaRule<TSelf>(keys),
    children_(subrules) {}

template <typename TSelf, typename ChildRule>
SchemaRuleParent<TSelf, ChildRule>::SchemaRuleParent(const YAML::Node& yml) {

    if (!yml.IsSequence()) {
        throw InvalidConfiguration("Sequence not found in YAML schema", Here());
    }

    for (const auto& elem : yml) {
        if (elem.IsSequence()) {
            children_.emplace_back(elem);
        } else {
            this->keys_.emplace_back(elem.as<std::string>());
        }
    }
}

template <typename TSelf, typename ChildRule>
void SchemaRuleParent<TSelf, ChildRule>::print(std::ostream& s) const {
    s << "[";
    {
        IndentStream indent(s, "  ");
        ::dasi::internal::print_list(s, this->keys_, "", "");
        s << "\n";
        for (const auto& r: children_) {
            r.print(s);
            s << "\n";
        }
    }
    s << "]";
}

// Explicitly instantiate only these instances in this unit

//template class SchemaRuleParent<SchemaRule3>;
//template class SchemaRuleParent<SchemaRule2>;
template class SchemaRule<SchemaRule3>;
template class SchemaRuleParent<SchemaRule2, SchemaRule3>;
template class SchemaRuleParent<SchemaRule1, SchemaRule2>;

// And the Schema proper

Schema::Schema(std::initializer_list<SchemaRule1> rules) :
    rules_(rules) {}

Schema::Schema(std::vector<SchemaRule1>&& rules) :
        rules_(std::move(rules)) {}

Schema::Schema(const YAML::Node& rules_yml) {
    // TODO: If 'schema' specifies a filename, load that instead.
    if (!rules_yml.IsSequence()) throw InvalidConfiguration("Schema configuration is not a sequence of rules", Here());
    rules_.insert(rules_.end(), rules_yml.begin(), rules_yml.end());
}

Schema::Schema(const std::vector<YAML::Node>& rules_yml) {
    rules_.insert(rules_.end(), rules_yml.begin(), rules_yml.end());
}

void Schema::print(std::ostream& o) const {
    o << "Schema[";
    {
        if (!rules_.empty()) o << "\n";
        for (const auto& r: rules_) {
            {
                IndentStream indent(o, "   ");
                o << " - ";
                r.print(o);
            }
            o << "\n";
        }
    }
    o << "]";
}

//----------------------------------------------------------------------------------------------------------------------

int get_stream_char(std::istream& s) {
    std::cout << "get char" << std::endl;
    int c;
    while ((c = s.get()) != EOF) {
        std::cout << ">> " << (char)c << " : " << (int)c << std::endl;
        if (c == '#') {
            while (true) {
                c = s.get();
                if (c == EOF || c == '\n') break;
            }
            if (c == EOF) break;
        } else {
            return c;
        }
    }
    return c;
}

int get_nonblank_stream_char(std::istream& s) {
    std::cout << "get nonblank " << s.tellg() << std::endl;
    int c;
    while ((c = get_stream_char(s)) != EOF) {
        std::cout << "-- " << (char)c << " : " << (int)c << std::endl;
        if (!::isspace(c)) return c;
    }
    return c;
}

Schema Schema::parse(std::istream& s) {

    auto rules_yml = YAML::LoadAll(s);
    return Schema(rules_yml);
}

Schema Schema::parse(const char* s) {
    std::istringstream input(s);
    return parse(input);
}

//----------------------------------------------------------------------------------------------------------------------

}