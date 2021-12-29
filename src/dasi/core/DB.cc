
#include "dasi/core/DB.h"
#include "dasi/core/Catalogue.h"

#include "dasi/api/Config.h"

using dasi::api::Config;


namespace dasi::core {

//----------------------------------------------------------------------------------------------------------------------

DB::DB(const OrderedReferenceKey& dbkey, const Config& config, bool reader) {
    std::string engine = config.getString("engine");
    if (reader) {
        catalogue_ = CatalogueFactory::instance().buildReader(engine, dbkey, config);
    } else {
        catalogue_ = CatalogueFactory::instance().buildWriter(engine, dbkey, config);
    }
}

DB::~DB() {}

void DB::archive(const SplitReferenceKey& key, const void* data, size_t length) {
    catalogue_->archive(key, data, length);
}

//----------------------------------------------------------------------------------------------------------------------

}