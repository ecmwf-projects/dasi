
#include "dasi/api/Dasi.h"

#include "dasi/core/Archiver.h"
#include "dasi/util/Exceptions.h"

#include "yaml-cpp/yaml.h"

#include <istream>

namespace dasi::api {

namespace {
    YAML::Node parseCharStar(const char* config) {
        std::istringstream iss(config);
        return YAML::Load(iss);
    }
}

//----------------------------------------------------------------------------------------------------------------------

Dasi::Dasi(std::istream& iss) :
    Dasi(YAML::Load(iss)) {}

Dasi::Dasi(const char* config) :
    Dasi(parseCharStar(config)) {}

Dasi::Dasi(const YAML::Node& config) :
    config_(new YAML::Node(config)) {
    if (!config_->IsMap()) throw util::InvalidConfiguration("Dasi configuration object not valid", Here());
    if (!(*config_)["schema"].IsDefined()) throw util::InvalidConfiguration("Dasi schema not specified in config", Here());
}

Dasi::~Dasi() {}

core::Schema& Dasi::schema() {
    if (!schema_) schema_ = std::make_unique<core::Schema>((*config_)["schema"]);
    return *schema_;
}

core::Archiver& Dasi::archiver() {

    if (!archiver_) {

        int archiverLRUsize = 20;
        auto lru_node = (*config_)["archiveLRUsize"];
        if (lru_node.IsDefined()) {
            try {
                archiverLRUsize = lru_node.as<int>();
            } catch (YAML::BadConversion& e) {
                throw util::InvalidConfiguration("archiveLRUsize not convertible to integer", Here());
            }
        }

        archiver_ = std::make_unique<core::Archiver>(schema(), archiverLRUsize);
    }
    return *archiver_;
}

void Dasi::archive(Key& key, const void* data, size_t length) {
    archiver().archive(key, data, length);
}


//----------------------------------------------------------------------------------------------------------------------

}