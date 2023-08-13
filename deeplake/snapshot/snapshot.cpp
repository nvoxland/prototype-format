#include "snapshot.hpp"
#include <fstream>
#include <nlohmann/json.hpp>
#include <iostream>
#include <set>

using json = nlohmann::json;

namespace deeplake {
    snapshot::snapshot(deeplake::branch branch, dataset *dataset) :
            version_(-1),
            branch_(branch),
            dataset_(dataset),
            protocol_(protocol_action(-1, -1)),
            metadata_(metadata_action("unset", std::nullopt, std::nullopt, 0)) {

        this->replay_log(branch, -1, -1, dataset);
    }

    std::string snapshot::path() {
        return dataset_->path();
    }

    long snapshot::version() { return version_; }

    deeplake::branch snapshot::branch() { return branch_; }

    protocol_action snapshot::protocol() { return protocol_; }

    metadata_action snapshot::metadata() { return metadata_; }

    std::vector<create_branch_action> snapshot::branches() { return branches_; }

    std::vector<add_file_action> snapshot::files() { return files_; }

    scan_result snapshot::scan() {
        return scan_result(dataset_->path(), files_);
    }

    void snapshot::replay_log(deeplake::branch branch, long from, long to, dataset *dataset) {
        if (!branch.is_main()) {
            this->replay_log(dataset->version_control()->branch_by_id(branch.from_id()), from, branch.from_version(), dataset);

            from = branch.from_version() + 1;
        }

        std::set<std::filesystem::path> sorted_paths;

        std::filesystem::path dir_path = {dataset->path() + "/_deeplake_log/"+branch.id()};
        if (std::filesystem::exists(dir_path)) {
            for (const auto &entry: std::filesystem::directory_iterator(dir_path)) {
                if (std::filesystem::is_regular_file(entry.path()) && entry.path().extension() == ".json") {
                    auto formatted_version = entry.path().filename().string().substr(0, entry.path().filename().string().length() - 5);
                    auto file_version = std::stol(formatted_version);
                    if (to != -1 && file_version > to) {
                        continue;
                    }
                    if (from == -1 || file_version >= from) {
                        sorted_paths.insert(entry.path());
                    }
                }
            }
        }

        for (const auto &path: sorted_paths) {
            std::ifstream ifs(path);

            json jsonArray = json::parse(ifs);
            for (auto &element: jsonArray) {
                if (element.contains("protocol")) {
                    protocol_ = deeplake::protocol_action(element);
                } else if (element.contains("metadata")) {
                    metadata_ = deeplake::metadata_action(element);
                } else if (element.contains("add")) {
                    files_.push_back(deeplake::add_file_action(element));
                } else if (element.contains("createBranch")) {
                    branches_.push_back(deeplake::create_branch_action(element));
                } else {
                    throw std::runtime_error("Unexpected action");
                }
            }

            version_ += 1;
        }
    }
}