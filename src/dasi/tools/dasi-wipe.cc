/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/*
 * This software was developed as part of the EC H2020 funded project IO-SEA
 * (Project ID: 955811) iosea-project.eu
 */

#include "dasi/api/Dasi.h"
#include "dasi/lib/LibDasi.h"
#include "dasi/tools/DASITool.h"
#include "eckit/exception/Exceptions.h"
#include "eckit/io/Buffer.h"
#include "eckit/io/MemoryHandle.h"
#include "eckit/log/Log.h"
#include "eckit/option/CmdArgs.h"
#include "eckit/option/SimpleOption.h"

using namespace eckit;
using namespace eckit::option;

namespace dasi::tools {

//----------------------------------------------------------------------------------------------------------------------

class DASIWipe: public DASITool {
public:  // methods
    DASIWipe(int argc, char** argv): DASITool(argc, argv) {
        options_.push_back(new SimpleOption<bool>("doit", "Delete the data and indexes"));
        options_.push_back(new SimpleOption<bool>("all", "Delete all (unowned) contents in a dirty database"));
        options_.push_back(new SimpleOption<bool>("porcelain", "List only the deleted"));
        options_.push_back(new SimpleOption<bool>("throw", "Throws when no data found"));
    }

private:  // methods
    void usage(const std::string& tool) const override;
    void init(const eckit::option::CmdArgs& args) override;
    void execute(const eckit::option::CmdArgs& args) override;

    [[nodiscard]]
    int numberOfPositionalArguments() const override {
        return 1;
    }

private:  // members
    bool doit_ {false};
    bool all_ {false};
    bool porcelain_ {false};
    bool throw_ {false};
};

//----------------------------------------------------------------------------------------------------------------------

void DASIWipe::usage(const std::string& tool) const {
    eckit::Log::info() << "\nUsage: " << tool << " [options] <query>\n\n\n"
                       << "Examples:\n=========\n\n"
                       << tool << " key1=value1,key2=value2\n\n";
    DASITool::usage(tool);
}

void DASIWipe::init(const eckit::option::CmdArgs& args) {
    doit_      = args.getBool("doit", false);
    all_       = args.getBool("all", false);
    porcelain_ = args.getBool("porcelain", false);
    throw_     = args.getBool("throw", false);
}

void DASIWipe::execute(const eckit::option::CmdArgs& args) {
    for (size_t i = 0; i < args.count(); ++i) {
        dasi::Query query(args(i));

        LOG_DEBUG_LIB(LibDasi) << " WIPE Query: " << query << "\n";

        int count = 0;
        for (auto&& entry : dasi().wipe(query, doit_, porcelain_, all_)) {
            count++;
            eckit::Log::info() << entry << "\n";
        }

        if (count == 0) {
            std::ostringstream oss;
            oss << "No data found for query: " << query;
            if (throw_) { throw DASIToolException(oss); }
            eckit::Log::info() << oss.str() << std::endl;
        }

        eckit::Log::info() << "Bye!" << std::endl;
    }
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace dasi::tools

int main(int argc, char** argv) {
    dasi::tools::DASIWipe app(argc, argv);
    return app.start();
}
