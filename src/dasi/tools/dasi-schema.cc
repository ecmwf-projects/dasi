/*
 * Copyright 2023- European Centre for Medium-Range Weather Forecasts (ECMWF).
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

#include "eckit/option/CmdArgs.h"
#include "eckit/option/SimpleOption.h"
#include "eckit/log/Log.h"
#include "eckit/filesystem/LocalPathName.h"
#include "eckit/parser/YAMLParser.h"

#include "dasi/tools/DASITool.h"
#include "dasi/api/Dasi.h"

#include <regex>

using namespace eckit::option;

namespace dasi::tools {

//----------------------------------------------------------------------------------------------------------------------

class DASISchema : public DASITool {
public:  // methods
    DASISchema(int argc, char** argv): DASITool(argc, argv) {
        options_.push_back(
            new SimpleOption<bool>("query", "Prints out the query string(s) (defined in the schema file)."));
        options_.push_back(new SimpleOption<bool>(
            "scan", "Prints out the keys found in the root paths (defined in the configuration yaml file)."));
    }

private:  // methods
    void usage(const std::string& tool) const override;

    void init(const eckit::option::CmdArgs& args) override;

    void execute(const eckit::option::CmdArgs& args) override;

    std::vector<std::vector<std::string>> tokenizeRules();

    void scan();

private:  // members
    bool query_ {false};
    bool scan_ {false};

    std::vector<std::vector<std::string>> rules_;
};

//----------------------------------------------------------------------------------------------------------------------

void DASISchema::usage(const std::string& tool) const {
    eckit::Log::info() << std::endl
                       << "Usage: " << tool << " [options]" << std::endl
                       << std::endl
                       << std::endl
                       << "Examples:" << std::endl
                       << "=========" << std::endl
                       << std::endl
                       << tool << " --config=dasi.yaml" << std::endl
                       << tool << " --query" << std::endl
                       << tool << " --scan" << std::endl
                       << std::endl;
    DASITool::usage(tool);
}

void DASISchema::init(const eckit::option::CmdArgs& args) {
    query_ = args.getBool("query", query_);
    scan_  = args.getBool("scan", scan_);
    if (query_ || scan_) {
        rules_ = tokenizeRules();
    } else {
        eckit::Log::info() << "No option is provided!" << std::endl;
        usage(name());
    }
}

void DASISchema::execute(const eckit::option::CmdArgs& /* args */) {
    // print out the rules in schema
    if (query_) {
        std::ostringstream oss;
        for (auto&& rule : rules_) {
            oss << "{";
            for (auto&& key : rule) {
                const char* separator = ",";
                if (&key == &rule.back()) { separator = ""; }
                oss << key << "=%s" << separator;
            }
            oss << "}" << std::endl;
        }
        // print out to std
        std::cout << oss.str();
    }

    if (scan_) { scan(); }
}

//----------------------------------------------------------------------------------------------------------------------

void DASISchema::scan() {
    using PathList = std::vector<eckit::LocalPathName>;

    // find the root paths defined in the config yaml
    const auto config = eckit::YAMLParser::decodeFile(configPath_);
    const auto roots  = config["spaces"]["handler"]["roots"];
    ASSERT(roots.isList());

    for (size_t i = 0; i < roots.size(); ++i) {
        PathList dirs;

        {
            PathList   files;
            const auto path = roots[i]["path"];
            eckit::LocalPathName(path).children(files, dirs);
        }

        for (auto&& dir : dirs) {
            std::string value;
            {
                // get the first value from the folder name
                std::stringstream ss;
                ss << (dir.baseName(false));
                std::getline(ss, value, ':');
            }
            for (auto&& rule : rules_) {
                dasi::Query query(rule[0] + "=" + value);
                for (const auto& elem : dasi().list(query)) {
                    elem.print(eckit::Log::info(), false);
                    eckit::Log::info() << eckit::newl;
                }
            }
        }
    }
}

std::vector<std::vector<std::string>> DASISchema::tokenizeRules() {
    std::vector<std::string> ruleLines;

    {
        std::stringstream ss;
        dasi().dumpSchema(ss);

        std::string line;
        while (std::getline(ss, line)) { ruleLines.push_back(line); }
    }

    std::vector<std::vector<std::string>> ruleList;

    for (auto&& rule : ruleLines) {
        // format
        rule.erase(0, 1);
        std::replace(rule.begin(), rule.end(), '[', ',');
        rule = std::regex_replace(rule, std::regex("[ \\]\\?]"), "");

        // tokenize (comma delimited)
        std::vector<std::string> tokens;
        {
            std::istringstream iss(rule);
            std::string        token;
            while (std::getline(iss, token, ',')) { tokens.push_back(token); }
        }

        ruleList.push_back(tokens);
    }

    return ruleList;
}

}  // namespace dasi::tools

//----------------------------------------------------------------------------------------------------------------------

int main(int argc, char** argv) {
    dasi::tools::DASISchema app(argc, argv);
    return app.start();
}
