
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>

#include "dasi/api/Dasi.h"
#include "dasi/util/Exceptions.h"
#include "dasi/tools/Parser.h"

using dasi::util::UserError;


void printList(dasi::api::ListResult& result) {
    for (const auto& key : result) {
        std::cout << key << std::endl;
    }
}

int main(int argc, char** argv) {
    const char* config_path = nullptr;
    const char* query_str = nullptr;

    try {
        for (char** argp = argv + 1; *argp != nullptr; ++argp) {
            if (strcmp(*argp, "-c") == 0 || strcmp(*argp, "--config") == 0) {
                config_path = *(++argp);
                if (config_path == nullptr) {
                    throw UserError("Argument needed for -c/--config", Here());
                }
            }
            else {
                if (query_str != nullptr) {
                    throw UserError("Too many arguments", Here());
                }
                query_str = *argp;
            }
        }
    }
    catch (const UserError& e) {
        std::cerr << e << std::endl;
        return 2;
    }

    if (config_path == nullptr) {
        config_path = std::getenv("DASI_CONFIG_FILE");
        if (config_path == nullptr) {
            std::cerr << "No configuration file. Please set DASI_CONFIG_FILE or pass -c/--config" << std::endl;
            return 2;
        }
        std::cout << "Config file: " << config_path << std::endl;
    }

    if (query_str == nullptr) {
        std::cout << "Usage: " << argv[0] << " [-c CONFIGFILE] <query>" << std::endl;
        return 1;
    }

    try {
        std::ifstream config(config_path);
        dasi::api::Dasi dasi(config);
        auto query = dasi::tools::parseQuery(query_str);
        std::cout << "Processing query " << query << std::endl;
        auto result = dasi.list(query);
        printList(result);
    }
    catch (const UserError& e) {
        std::cerr << e << std::endl;
        return 1;
    }

    return 0;
}