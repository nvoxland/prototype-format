#include <string>
#include "dataset.hpp"
#include <filesystem>

using namespace std;

namespace deeplake {
    dataset::dataset(const string &path) : path_(path) {}

    dataset::dataset(const filesystem::path &path): path_(path.string()){}


    const std::string &dataset::path() const {
        return path_;
    }

    dataset create(const string &path) {
        if (filesystem::exists(path)) {
            throw runtime_error("'" + path + "' already exists");
        }

        filesystem::create_directories(path);
        filesystem::create_directories(path + "/_deeplake_log");

        return dataset(path);
    }
}