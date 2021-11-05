
#include "dasi/api/Dasi.h"

#include "dasi/core/ArchiveVisitor.h"
#include "dasi/util/Exceptions.h"

#include "yaml-cpp/yaml.h"

#include <istream>

namespace dasi {

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
    if (!config.IsMap()) throw InvalidConfiguration("Dasi configuration object not valid", Here());
}

void Dasi::archive(Key& key, const void* data, size_t length) {
    ArchiveVisitor visitor(data, length);
    schema_.walk(key, visitor);
}


//----------------------------------------------------------------------------------------------------------------------

}