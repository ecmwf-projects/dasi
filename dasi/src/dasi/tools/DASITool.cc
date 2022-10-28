
#include "dasi/tools/DASITool.h"

#include "eckit/exception/Exceptions.h"
#include "eckit/option/CmdArgs.h"


namespace dasi::tools {

//-------------------------------------------------------------------------------------------------

static DASITool* instance_ = nullptr;

DASITool::DASITool(int argc, char** argv) :
    eckit::Tool(argc, argv) {}

static void usage(const std::string& tool) {
    ASSERT(instance_);
    instance_->usage(tool);
}

void DASITool::run() {
    eckit::option::CmdArgs args(&dasi::tools::usage,options_,
                                numberOfPositionalArguments(),
                                minimumPositionalArguments());

    init(args);
    execute(args);
    finish(args);
}

//-------------------------------------------------------------------------------------------------

} // namespace dasi::tools
