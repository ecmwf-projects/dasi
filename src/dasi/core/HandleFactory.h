
#pragma once

#include <map>
#include <string>

namespace dasi::api {
    class ReadHandle;
}

namespace dasi::core {

//----------------------------------------------------------------------------------------------------------------------

class HandleBuilderBase {

public: // types

    using offset_type = long long;
    using length_type = long long;  // -1 means unlimited length, if applicable

public: // methods

    HandleBuilderBase(std::string name);
    virtual ~HandleBuilderBase();

    virtual api::ReadHandle* makeReadHandle(const std::string& location, offset_type offset, length_type length) = 0;

private: // members

    std::string name_;

};

//----------------------------------------------------------------------------------------------------------------------

class HandleFactory {

public: // methods

    static HandleFactory& instance();

    void add(const std::string& name, HandleBuilderBase* builder);
    void remove(const std::string& name);

    api::ReadHandle* buildReadHandle(const std::string& name, const std::string& location,
        HandleBuilderBase::offset_type offset, HandleBuilderBase::length_type length);

private: // methods

    HandleFactory();

private: // members

    std::map<std::string, HandleBuilderBase*> builders_;

};

//----------------------------------------------------------------------------------------------------------------------

}