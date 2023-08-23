#
# Copyright 2023- European Centre for Medium-Range Weather Forecasts (ECMWF)
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     https://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

# NOTE:
# In case, DASI and its dependencies are not installed.
# CMAKE_PREFIX_PATH=/somedir/bundle_dasi/build/Debug cmake [options]

cmake_minimum_required(VERSION 3.12 FATAL_ERROR)

project(forecast VERSION 0.1 LANGUAGES C)

set(CMAKE_C_STANDARD 90)

find_package(dasi REQUIRED)

find_package(eckit 1.20 REQUIRED)

# forecast

add_executable( forecast forecast.c )

target_link_libraries( forecast PUBLIC dasi )

# mean

add_executable( mean mean.c )

target_link_libraries( mean PUBLIC dasi )
