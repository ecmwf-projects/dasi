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

#include "dasi/api/Key.h"
#include "dasi/api/Query.h"
#include "dasi/api/detail/ListDetail.h"
#include "dasi/api/detail/PolicyDetail.h"
#include "dasi/api/detail/RetrieveDetail.h"

#include <memory>

namespace dasi {

//-------------------------------------------------------------------------------------------------

class DasiImpl;

class Dasi {

public: // methods

    /// Dasi Constructor.
    /// @param dasi_config A YAML configuration, or configuration path for Dasi. Specifies where data is stored
    /// @param application_config A configuration for runtime-specific overrides within Dasi. Controls how data is written/read
    explicit Dasi(const char* dasi_config, const char* application_config=nullptr);

    ~Dasi();

    /// Write data to be stored according to Dasi configuration
    /// @note After this call returns, Dasi has written this data to a backend or taken an internal copy. It is not
    ///       guaranteed accessible, or persisted wrt. failure, until flush() is called.
    /// @param key The metadata description of the data to store and index
    /// @param data A pointer to a (read-only) copy of the data
    /// @param length The length of the data to store in bytes
    void archive(const Key& key, const void* data, size_t length);

    /// List data present and retrievable from the archive
    /// @param query A description of the span of metadata to list within
    /// @returns An iterable generator object of ListElements, containing details of the objects found, the
    ///          keys describing them and a timestamp of object archival.
    ListGenerator list(const Query& query);

    /// Retrieve data objects from the archive
    /// @todo - a retrieve function that gets data sizes as well.
    ///         Support explicit data locations for lazy retrievals (e.g. nice xarray or dask support)
    ///         Build on the Query functionality?
    /// @todo - Explicitly and cleanly distinguish _all_ from _unspecified_ (currently all cases treated as _unspecified_)
    /// @param query A description of the span of data to retrieve
    /// @returns A generic data handle, that will retrieve the data.
    RetrieveResult retrieve(const Query& query);

    /// Flushes all buffers and ensures all internal state is safe wrt. failure
    /// @note always safe to call
    void flush();

    /// Set a named policy, or set of policies, for the data collections identified by the query
    /// @param query The data collections to modify
    /// @param policyDict A (nested) dictionary of policy keys/values to set
    /// @returns Identified policy objects for each identified data collection, relative to the specified name.
    ///          Corresponds to the output from queryPolicy
    PolicyGenerator setPolicy(const Query& query, const PolicyDict& policyDict);

    /// Retrieve a named policy, or set of policies, for the data collections identified by the query
    /// @param query The data collections to modify
    /// @param name The name of the policy to set, or an identifier of the subset of policies to set
    /// @returns Identified policy objects for each identified data collection, relative to the specified name
    PolicyGenerator queryPolicy(const Query& query, const std::string& name="");

    /// @note - move should follow same api and/or an ----- ioctl-type ----- api

private: // members

    std::unique_ptr<DasiImpl> impl_;
};

//-------------------------------------------------------------------------------------------------

} // namespace dasi


