# (C) Copyright 2011- ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
# In applying this licence, ECMWF does not waive the privileges and immunities
# granted to it by virtue of its status as an intergovernmental organisation nor
# does it submit to any jurisdiction.

########################################################################################################################

cmake_minimum_required( VERSION 3.12 FATAL_ERROR )

find_package( ecbuild 3.6 REQUIRED HINTS ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/../ecbuild ${CMAKE_CURRENT_SOURCE_DIR}/../../ecsdk/ecbuild)

project( DASIBundle LANGUAGES C CXX )

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

########################################################################################################################

ecbuild_bundle_initialize()

ecbuild_bundle( PROJECT eckit        GIT "ssh://git@git.ecmwf.int/ecsdk/eckit"       BRANCH develop UPDATE)
ecbuild_bundle( PROJECT metkit       GIT "ssh://git@git.ecmwf.int/ecsdk/metkit"      BRANCH develop UPDATE)
ecbuild_bundle( PROJECT fdb5         GIT "ssh://git@git.ecmwf.int/mars/fdb5"         BRANCH feature/FDB-296 UPDATE)
ecbuild_bundle( PROJECT dasi         GIT "ssh://git@git.ecmwf.int/mars/dasi"         BRANCH develop UPDATE)

ecbuild_bundle_finalize()
