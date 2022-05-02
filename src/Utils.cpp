#include "Utils.h"

#include <sstream>
#include <fstream>
#include <cstdlib>

#include "config.h"

namespace fs = std::filesystem;

namespace {
    const char *ENV_APP_RESOURCES_DIR = "APP_RESOURCES_DIR";
}


std::string Utils::resolve_path(const std::string_view &file_path) {
    fs::path fpath{file_path};
    if (fpath.is_absolute()) {
        return fpath.string();
    }

    fpath = fpath.lexically_relative("./");

    if (Config::root_folder.compare("") != 0) {
        return (Config::root_folder / fpath).make_preferred().lexically_normal().string();
    }

    fs::path env_path{std::getenv(ENV_APP_RESOURCES_DIR)};
    if (env_path != "") {
        return (env_path / fpath).make_preferred().lexically_normal().string();
    }

    return fpath.lexically_normal().string();
}

std::string Utils::read_file(const std::string_view &path) {
    std::stringstream ss{};
    std::ifstream file{static_cast<std::string>(path)};
    if (!file.is_open()) {
        // THROW: ...
    }
    ss << file.rdbuf();
    return ss.str();
}
