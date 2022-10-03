/*
 * (C) Copyright 2022- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


/// @author Simon Smart
/// @date   Sep 2022

#pragma once

#include "dasi/Key.h"
//#include "dasi/Query.h"

#include <memory>

namespace dasi {

//-------------------------------------------------------------------------------------------------

class DasiImpl;

class Dasi {

public: // methods

    /** Dasi Constructor.
     *  \param dasi_config A YAML configuration for Dasi. Specifies where data is stored
     *  \param application_config A configuration for runtime-specific overrides within Dasi. Controls how data is written/read
     */
    explicit Dasi(const char* dasi_config, const char* application_config=nullptr);

    ~Dasi();

    void archive(const Key& key, const void* data, size_t length);

    //ListGenerator list(const Query& request);

private: // members

    std::unique_ptr<DasiImpl> impl_;
};

//-------------------------------------------------------------------------------------------------

} // namespace dasi


