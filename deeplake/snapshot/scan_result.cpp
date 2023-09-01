#include "scan_result.hpp"
#include "../deeplog/actions/add_file_action.hpp"

#include <fstream>

namespace deeplake {
    scan_result::scan_result(std::vector<std::string> data) : data_(data) {

    }

    scan_result::scan_result(std::string base_path, std::vector<add_file_action> files) : data_(std::vector<std::string>()) {
        for (add_file_action add_file: files) {
            auto file_stream = std::ifstream(base_path + "/" + add_file.path());
            std::string line;
            while (std::getline(file_stream, line)) {
                data_.emplace_back(line);
            }
        }
    }

    std::string &scan_result::operator[](int idx) {
        return data_[idx];
    }

    std::vector<std::string>::iterator scan_result::begin() {
        return data_.begin();
    }

    std::vector<std::string>::iterator scan_result::end() {
        return data_.end();
    }
}

