
#include "dasi/tools/Parser.h"

#include <algorithm>
#include <sstream>
#include <vector>

#include "dasi/util/Exceptions.h"

using dasi::util::UserError;


namespace dasi::tools {

dasi::api::Key parseKey(const std::string& s) {
    dasi::api::Key key;

    for (auto it = s.begin(); it != s.end();) {
        auto kend = std::find(it, s.end(), '=');
        if (kend == s.end()) {
            std::ostringstream oss;
            oss << "Invalid key '" << s << "'";
            throw UserError(oss.str(), Here());
        }
        std::string k(it, kend);
        it = kend;
        auto vend = std::find(++it, s.end(), ',');
        std::string v(it, vend);
        it = vend;
        if (it != s.end()) {
            ++it;
        }
        key.set(k, v);
    }

    return key;
}

dasi::api::Query parseQuery(const std::string& s) {
    dasi::api::Query query;

    for (auto it = s.begin(); it != s.end();) {
        auto kend = std::find(it, s.end(), '=');
        if (kend == s.end()) {
            std::ostringstream oss;
            oss << "Invalid key '" << s << "'";
            throw UserError(oss.str(), Here());
        }
        std::string k(it, kend);
        it = kend;
        ++it;
        std::vector<std::string> v;
        while (it != s.end()) {
            const auto seps = {'/', ','};
            auto vend = std::find_first_of(it, s.end(), seps.begin(), seps.end());
            v.emplace_back(it, vend);
            it = vend;
            if (it != s.end()) {
                if (*it == ',') {
                    break;
                }
                ++it;
            }
        }
        if (it != s.end()) {
            ++it;
        }
        query.set(k, v);
    }

    return query;
}

}