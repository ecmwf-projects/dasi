
#pragma once

#include <memory>


namespace dasi::core {

class Catalogue;

//----------------------------------------------------------------------------------------------------------------------

class DB {

public: // methods

    DB();
    virtual ~DB();

private: // members

    std::unique_ptr<Catalogue> catalogue_;
};

//----------------------------------------------------------------------------------------------------------------------

}