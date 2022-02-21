
#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <fstream>
#include <memory>
#include <sstream>
#include <utility>
#include <vector>

#include "dasi/api/Dasi.h"
#include "dasi/api/ReadHandle.h"

#include "dasi/util/AutoCloser.h"
#include "dasi/util/Exceptions.h"

using dasi::util::UserError;


dasi::api::Query parseQuery(const std::string& s) {
    dasi::api::Query query;

    for (auto it = s.begin(); it != s.end();) {
        auto kend = std::find(it, s.end(), '=');
        if (kend == s.end()) {
            std::ostringstream oss;
            oss << "Invalid key '" << s << "'";
            throw UserError(oss.str(), Here());
        }
        std::string k(it, kend);
        it = kend;
        ++it;
        std::vector<std::string> v;
        while (it != s.end()) {
            const auto seps = {'/', ','};
            auto vend = std::find_first_of(it, s.end(), seps.begin(), seps.end());
            v.emplace_back(it, vend);
            it = vend;
            if (it != s.end()) {
                if (*it == ',') {
                    break;
                }
                ++it;
            }
        }
        if (it != s.end()) {
            ++it;
        }
        query.set(k, v);
    }

    return query;
}

void writeData(const char* filename, dasi::api::RetrieveResult& result, size_t bufsize = BUFSIZ) {
    std::unique_ptr<dasi::api::ReadHandle> handle(result.toHandle());
    handle->open();
    dasi::util::AutoCloser closer(*handle);
    char buf[bufsize];
    std::ofstream stream(filename, std::ios::binary);
    size_t nread;
    while ((nread = handle->read(buf, bufsize)) > 0) {
        stream.write(buf, nread);
    }
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
                auto query = *argp;
                auto file = *(++argp);
                args.emplace_back(query, file);
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
        std::cout << "Usage: " << argv[0] << " [-c CONFIGFILE] <query1> <file1> [<key2> <file2> ...]" << std::endl;
        return 1;
    }

    try {
        std::ifstream config(config_path);
        dasi::api::Dasi dasi(config);
        for (const auto& arg : args) {
            auto query = parseQuery(arg.first);
            std::cout << "Processing query " << query << std::endl;
            auto result = dasi.retrieve(query);
            writeData(arg.second, result);
        }
    }
    catch (const UserError& e) {
        std::cerr << e << std::endl;
        return 1;
    }

    return 0;
}