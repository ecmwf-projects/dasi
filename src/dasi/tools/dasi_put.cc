
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <fstream>
#include <utility>
#include <vector>

#include "dasi/api/Dasi.h"
#include "dasi/util/Buffer.h"
#include "dasi/util/Exceptions.h"
#include "dasi/tools/Parser.h"

using dasi::util::UserError;


dasi::util::Buffer readData(const char* filename) {
    std::ifstream stream(filename, std::ios::binary);
    stream.seekg(0, std::ios_base::end);
    size_t len = stream.tellg();
    stream.seekg(0);
    dasi::util::Buffer buf(len);
    stream.read(static_cast<char*>(buf.data()), len);
    return buf;
}


int main(int argc, char** argv) {
    const char* config_path = nullptr;
    std::vector<std::pair<const char*, const char*>> args;

    try {
        for (char** argp = argv + 1; *argp != nullptr; ++argp) {
            if (strcmp(*argp, "-c") == 0 || strcmp(*argp, "--config") == 0) {
                config_path = *(++argp);
                if (config_path == nullptr) {
                    throw UserError("Argument needed for -c/--config", Here());
                }
            }
            else {
                auto key = *argp;
                auto file = *(++argp);
                args.emplace_back(key, file);
                if (*argp == nullptr) {
                    throw UserError("Missing data file", Here());
                }
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

    if (args.empty()) {
        std::cout << "Usage: " << argv[0] << " [-c CONFIGFILE] <key1> <file1> [<key2> <file2> ...]" << std::endl;
        return 1;
    }

    try {
        std::ifstream config(config_path);
        dasi::api::Dasi dasi(config);
        for (const auto& arg : args) {
            auto key = dasi::tools::parseKey(arg.first);
            std::cout << "Archiving " << key << std::endl;
            auto buf = readData(arg.second);
            dasi.archive(key, buf.data(), buf.size());
        }
    }
    catch (const UserError& e) {
        std::cerr << e << std::endl;
        return 1;
    }

    return 0;
}