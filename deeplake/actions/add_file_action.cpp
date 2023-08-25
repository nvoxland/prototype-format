#include "add_file_action.hpp"

namespace deeplake {
    add_file_action::add_file_action(std::string path, long size, long modification_time, bool data_change):
            path_(path), size_(size), modification_time_(modification_time), data_change_(data_change) {}

    add_file_action::add_file_action(const nlohmann::json &j) {
        const auto &base = j.at("add");
        base.at("path").get_to(path_);
        base.at("size").get_to(size_);
        base.at("modificationTime").get_to(modification_time_);
        base.at("dataChange").get_to(data_change_);
    }

    std::string add_file_action::path() { return path_; }

    long add_file_action::size() { return size_; }

    long add_file_action::modification_time() { return modification_time_; }

    void add_file_action::to_json(nlohmann::json &j) {
        j["add"]["path"] = path_;
        j["add"]["size"] = size_;
        j["add"]["modificationTime"] = modification_time_;
        j["add"]["dataChange"] = data_change_;
    }}