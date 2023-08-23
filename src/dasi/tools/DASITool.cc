
#include "dasi/tools/DASITool.h"

#include "eckit/exception/Exceptions.h"
#include "eckit/option/CmdArgs.h"
#include "eckit/option/SimpleOption.h"

using namespace eckit::option;

namespace dasi::tools {

//-------------------------------------------------------------------------------------------------

static DASITool* instance_ = nullptr;

DASITool::DASITool(int argc, char** argv): eckit::Tool(argc, argv) {
    ASSERT(instance_ == nullptr);
    instance_ = this;
    options_.push_back(new SimpleOption<std::string>("config", "DASI Configuration file (yaml)"));
}

static void usage(const std::string& tool) {
    ASSERT(instance_);
    instance_->usage(tool);
}

void DASITool::run() {
    eckit::option::CmdArgs args(&dasi::tools::usage, options_, numberOfPositionalArguments(),
                                minimumPositionalArguments());

    initInternal(args);
    init(args);
    execute(args);
    finish(args);
}

void DASITool::initInternal(const eckit::option::CmdArgs& args) {
    configPath_ = args.getString("config", "./dasi.yml");
}

Dasi& DASITool::dasi() {
    if (!dasi_) { dasi_.emplace(configPath_.localPath()); }
    return dasi_.value();
}

//-------------------------------------------------------------------------------------------------

}  // namespace dasi::tools
