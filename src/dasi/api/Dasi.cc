
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
    schema_(config["schema"]) {
    if (!config.IsMap()) throw util::InvalidConfiguration("Dasi configuration object not valid", Here());
}

Dasi::~Dasi() {}

core::Archiver& Dasi::archiver() {
    if (!archiver_) archiver_ = std::make_unique<core::Archiver>(schema_);
    return *archiver_;
}

void Dasi::archive(Key& key, const void* data, size_t length) {
    archiver().archive(key, data, length);
}


//----------------------------------------------------------------------------------------------------------------------

}