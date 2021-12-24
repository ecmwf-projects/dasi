
#pragma once

#include <mutex>
#include <map>
#include <string>
#include <iosfwd>


namespace dasi::core {

//----------------------------------------------------------------------------------------------------------------------

class Catalogue {

public: // methods

    Catalogue() = default;
    virtual ~Catalogue() = default;
};

//----------------------------------------------------------------------------------------------------------------------

class CatalogueBuilderBase {

protected: // methods

    explicit CatalogueBuilderBase(const char* name);
    virtual ~CatalogueBuilderBase();

public: // methods

    [[ nodiscard ]]
    virtual Catalogue* make() const = 0;

private: // members

    const char* name_;
};

//----------------------------------------------------------------------------------------------------------------------

template <typename CATALOGUE>
class CatalogueBuilder : public CatalogueBuilderBase {

public: // methods

    explicit CatalogueBuilder() : CatalogueBuilderBase(CATALOGUE::name) {}
    ~CatalogueBuilder() override = default;

private: // methods

    [[ nodiscard ]]
    Catalogue* make() const override { return new CATALOGUE{}; }
};

//----------------------------------------------------------------------------------------------------------------------

class CatalogueFactory {

public: // methods

    static CatalogueFactory& instance();

    void enregister(const char* name, const CatalogueBuilderBase* builder);
    void deregister(const char* name);

    Catalogue* build(const std::string& name);

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