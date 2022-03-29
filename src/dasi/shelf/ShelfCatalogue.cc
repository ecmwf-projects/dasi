
#include "dasi/shelf/ShelfCatalogue.h"

#include "dasi/api/Config.h"

#include "dasi/core/FileReadHandle.h"
#include "dasi/core/SplitReferenceKey.h"

#include <cctype>
#include <fstream>
#include <iomanip>
#include <sstream>

namespace dasi::shelf {

static std::string encode(const std::string_view& v) {
    std::ostringstream r;
    r.fill('0');
    r << std::hex;

    for (const auto c : v) {
        auto cu = static_cast<unsigned char>(c);
        if (std::isalnum(cu) || c == '-' || c == '.' || c == '_')
            r << c;
        else
            r << ':' << std::setw(2) << int(cu);
    }

    return r.str();
}

ShelfCatalogue::ShelfCatalogue(const core::OrderedReferenceKey& dbkey, const api::Config& config) :
    Catalogue(dbkey, config),
    root_(config.getString("root")) {}


void ShelfCatalogue::archive(const core::SplitReferenceKey& key, const void* data, size_t length) {
    ASSERT(key[0] == dbkey());
    auto path = buildPath(key);
    std::filesystem::create_directories(path.parent_path());
    std::ofstream fout(path, std::ios::out | std::ios::binary);
    fout.write(reinterpret_cast<const char*>(data), length);
}

api::ObjectLocation ShelfCatalogue::retrieve(const core::SplitReferenceKey& key) {
    ASSERT(key[0] == dbkey());
    auto path = buildPath(key);
    if (!std::filesystem::exists(path)) {
        std::ostringstream oss;
        oss << key;
        throw util::ObjectNotFound(oss.str(), Here());
    }
    return core::FileReadHandle::toLocation(path);
}


std::filesystem::path ShelfCatalogue::buildPath(const core::SplitReferenceKey& key) {
    std::filesystem::path p(root_);
    for (int level = 0; level < 3; ++level) {
        std::ostringstream comp;
        char sep[2] = {0, 0};
        for (const auto& kv : key[level]) {
            comp << sep << encode(kv.first) << '=' << encode(kv.second);
            sep[0] = ',';
        }
        p /= comp.str();
    }
    return p;
}


core::CatalogueBuilder<ShelfCatalogue> shelfBuilder;

}
