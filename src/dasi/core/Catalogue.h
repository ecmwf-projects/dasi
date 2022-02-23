
#pragma once

#include "dasi/core/OrderedReferenceKey.h"

#include "dasi/core/OrderedKey.h"

#include <mutex>
#include <map>
#include <string>
#include <iosfwd>
#include <memory>

namespace dasi::api {
class Config;
class ReadHandle;
}

namespace dasi::core {

class SplitReferenceKey;

//----------------------------------------------------------------------------------------------------------------------

class Catalogue {

public: // methods

    explicit Catalogue(const OrderedReferenceKey& dbkey, const api::Config& config);
    virtual ~Catalogue() = default;

    [[ nodiscard ]]
    const OrderedKey& dbkey() const { return dbkey_; };

    [[ nodiscard ]]
    const api::Config& config() const { return config_; }

    virtual void archive(const SplitReferenceKey& key, const void* data, size_t length) { NOTIMP; }
    virtual api::ReadHandle* retrieve(const SplitReferenceKey& key) { NOTIMP; }

private: // members

    virtual void print(std::ostream& s) const { s << "Catalogue[UNKNOWN]"; }

private: // friend

    friend std::ostream& operator<<(std::ostream& s, const Catalogue& c) {
        c.print(s);
        return s;
    }

private: // members

    OrderedKey dbkey_;
    const api::Config& config_;
};

//----------------------------------------------------------------------------------------------------------------------

class CatalogueBuilderBase {

protected: // methods

    explicit CatalogueBuilderBase(const char* name);
    virtual ~CatalogueBuilderBase();

public: // methods

    [[ nodiscard ]]
    virtual std::unique_ptr<Catalogue> makeReader(const OrderedReferenceKey& key, const api::Config& config) const = 0;

    [[ nodiscard ]]
    virtual std::unique_ptr<Catalogue> makeWriter(const OrderedReferenceKey& key, const api::Config& config) const = 0;

private: // members

    const char* name_;
};

//----------------------------------------------------------------------------------------------------------------------

template <typename CATALOGUE_READER, typename CATALOGUE_WRITER=CATALOGUE_READER>
class CatalogueBuilder : public CatalogueBuilderBase {

public: // methods

    CatalogueBuilder() : CatalogueBuilderBase(CATALOGUE_READER::name) {}
    explicit CatalogueBuilder(const char* name) : CatalogueBuilderBase(name) {}
    ~CatalogueBuilder() override = default;

private: // methods

    [[ nodiscard ]]
    std::unique_ptr<Catalogue> makeReader(const OrderedReferenceKey& key, const api::Config& config) const override {
        return std::make_unique<CATALOGUE_READER>(key, config);
    }

    [[ nodiscard ]]
    std::unique_ptr<Catalogue> makeWriter(const OrderedReferenceKey& key, const api::Config& config) const override {
        return std::make_unique<CATALOGUE_WRITER>(key, config);
    }
};

//----------------------------------------------------------------------------------------------------------------------

class CatalogueFactory {

public: // methods

    static CatalogueFactory& instance();

    void enregister(const char* name, const CatalogueBuilderBase* builder);
    void deregister(const char* name);

    std::unique_ptr<Catalogue> buildReader(const std::string& name,
                                           const OrderedReferenceKey& key,
                                           const api::Config& config);
    std::unique_ptr<Catalogue> buildWriter(const std::string& name,
                                           const OrderedReferenceKey& key,
                                           const api::Config& config);

    void list(std::ostream& s, const char* sep=", ");

private: // metods

    CatalogueFactory() = default;
    ~CatalogueFactory() = default;

private: // members

    std::mutex m_;
    std::map<std::string, const CatalogueBuilderBase*> builders_;
};

//----------------------------------------------------------------------------------------------------------------------

}