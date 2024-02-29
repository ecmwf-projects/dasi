/* Copyright 2024- European Centre for Medium-Range Weather Forecasts (ECMWF)
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     https://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/*
 * This software was developed as part of the EC H2020 funded project IO-SEA
 * (Project ID: 955811) iosea-project.eu
 */

/// @file   Utils.h
/// @author Metin Cakircali
/// @date   Feb 2024

#pragma once

#include <iostream>
#include <map>
#include <random>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace weather {

using DataContainer = std::map<std::pair<std::string, int>, std::vector<double>>;

namespace utils {

std::string getInputString(const std::vector<std::string>& args, const std::string& parShort,
                           const std::string& parLong, const std::string& defaultValue = "") {
    for (auto it = args.begin(), end = args.end(); it != end; ++it) {
        if (*it == parShort || *it == parLong) {
            if (it + 1 == end) { break; }
            return *(it + 1);
        }
    }

    if (defaultValue.empty()) { throw std::runtime_error("Missing input: " + parShort + "/" + parLong + " <value>"); }

    return defaultValue;
}

int getInputInteger(const std::vector<std::string>& args, const std::string& parShort, const std::string& parLong,
                    const int defaultValue = 0) {
    int result = defaultValue;

    std::istringstream iss(getInputString(args, parShort, parLong));

    if (!(iss >> result)) { throw std::runtime_error("Invalid number: " + iss.str()); }

    return result;
}

auto generateRandomData(const uint8_t size) -> std::vector<double> {
    std::vector<double> result;
    result.reserve(size);

    std::random_device rd;

    std::mt19937 gen(rd());

    std::normal_distribution<double> dist(5.0, 2.0);

    for (uint8_t i = 0; i < size; i++) { result.emplace_back(dist(gen)); }

    return result;
}

auto getWeatherData(const uint8_t size) -> DataContainer {
    DataContainer result;

    // meta data
    const auto levels = {1, 3, 5, 7};
    const auto params = {"pressure", "temperature"};

    // bulk data
    for (auto&& param : params) {
        for (auto&& level : levels) {
            const auto& data = weather::utils::generateRandomData(size);
            result.emplace(std::pair(param, level), data);
        }
    }

    return result;
}

}  // namespace utils

}  // namespace weather
