
#pragma once

#include "dasi/core/Catalogue.h"

#include <filesystem>

namespace dasi::core {
class SplitReferenceKey;
}

namespace dasi::shelf {

class ShelfCatalogue : public dasi::core::Catalogue {

public: // types

    constexpr static const char* name = "shelf";

public: // methods

    explicit ShelfCatalogue(const core::OrderedReferenceKey& dbkey, const api::Config& config);

private: // methods

    void archive(const core::SplitReferenceKey& key, const void* data, size_t length) override;

    void print(std::ostream& s) const override {
        s << "ShelfCatalogue[root=" << root_ << "]";
    }

    std::filesystem::path buildPath(const core::SplitReferenceKey& key);

private: // members

    const std::filesystem::path root_;
};

}