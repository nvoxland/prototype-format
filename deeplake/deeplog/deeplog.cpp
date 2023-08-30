#include "deeplog.hpp"
#include <filesystem>
#include <set>
#include <fstream>
#include "nlohmann/json.hpp"
#include "actions/protocol_action.hpp"
#include "actions/metadata_action.hpp"
#include "actions/create_branch_action.hpp"

namespace deeplake {

    deeplog::deeplog(std::string path) : path_(path), all_branches_(std::make_shared<std::vector<deeplake::branch>>()) {
        read_main();
    };

    std::shared_ptr<deeplake::deeplog> deeplog::create(std::string path) {
        if (std::filesystem::exists(path)) {
            throw std::runtime_error("'" + path + "' already exists");
        }

        std::filesystem::create_directories(path);
        std::filesystem::create_directories(path + "/_deeplake_log");

        return open(path);
    }

    std::shared_ptr<deeplake::deeplog> deeplog::open(std::string path) {
        const deeplake::deeplog &log = deeplog(path);
        return std::make_shared<deeplake::deeplog>(log);
    }

    std::unique_ptr<deeplake::snapshot> deeplog::snapshot(const deeplake::branch &branch, const std::optional<long> &version) {
        std::vector<std::filesystem::path> sorted_paths = list_files(branch, 0, version);

        std::vector<std::string> snapshot_files = {};

        for (const auto &path: sorted_paths) {
            std::ifstream ifs(path);

            nlohmann::json jsonArray = nlohmann::json::parse(ifs);
            for (auto &element: jsonArray) {
                if (element.contains("add")) {
                    auto add = deeplake::add_file_action(element);
                    snapshot_files.push_back(add.path());
                }
            }
        }

        long snapshot_version = -1;
        if (!sorted_paths.empty()) {
            snapshot_version = file_version(sorted_paths.back());
        }

        return std::make_unique<deeplake::snapshot>(deeplake::snapshot(branch, snapshot_version, snapshot_files, shared_from_this()));
    }

    std::shared_ptr<deeplake::protocol> deeplog::protocol() const {
        return protocol_;
    }

    std::shared_ptr<deeplake::metadata> deeplog::metadata() const {
        return metadata_;
    }

    void deeplog::commit(const deeplake::branch &branch,
                         const long &base_version,
                         const std::vector<deeplake::action *> &actions) {
        nlohmann::json commit_json;

        for (auto action: actions) {
            nlohmann::json action_json = nlohmann::json::object();
            action->to_json(action_json);
            commit_json.push_back(action_json);
        }

        std::ostringstream ss;
        ss << std::setw(20) << std::setfill('0') << (base_version + 1);

        auto log_dir = path_ + "/_deeplake_log/" + branch.id() + "/";

        std::filesystem::create_directories(log_dir);

        auto operationFilePath = log_dir + ss.str() + ".json";

        std::fstream file(operationFilePath, std::ios::out);
        if (!file.is_open()) {
            throw std::runtime_error("Error opening file: " + operationFilePath);
        }

        file << commit_json;
        file.close();

        if (branch.is_main()) {
            read_main();
        }

        //        dataset_->update();
    }

    std::shared_ptr<std::vector<branch>> deeplog::all_branches() const {
        return all_branches_;
    }

    deeplake::branch deeplog::branch(const std::string branch_name) const {
        auto branch = std::ranges::find_if(*all_branches_,
                                           [branch_name](deeplake::branch b) { return b.name() == branch_name; });
        if (branch == all_branches_->end()) {
            throw std::runtime_error("Branch '" + branch_name + "' not found");
        }
        return *branch;
    }

    deeplake::branch deeplog::branch_by_id(const std::string branch_id) const {
        auto branch = std::ranges::find_if(*all_branches_,
                                           [branch_id](deeplake::branch b) { return b.id() == branch_id; });
        if (branch == all_branches_->end()) {
            throw std::runtime_error("Branch id '" + branch_id + "' not found");
        }
        return *branch;
    }

    long deeplake::deeplog::branch_version(const deeplake::branch &branch) {
        return file_version(list_files(branch, 0, std::nullopt).back());
    }

    std::vector<std::filesystem::path> deeplog::list_files(deeplake::branch branch,
                                                           std::optional<long> from,
                                                           std::optional<long> to) {
        std::vector<std::filesystem::path> return_files = {};

        if (!branch.is_main()) {
            for (const auto &file: list_files(branch_by_id(branch.from_id()), from, branch.from_version())) {
                return_files.push_back(file);
            }

            from = branch.from_version() + 1;
        }

        std::set<std::filesystem::path> sorted_paths = {};

        std::filesystem::path dir_path = {path_ + "/_deeplake_log/"};
        if (std::filesystem::exists(dir_path)) {
            for (const auto &entry: std::filesystem::directory_iterator(dir_path)) {
                if (std::filesystem::is_regular_file(entry.path()) && entry.path().extension() == ".json") {
                    auto found_version = file_version(entry.path());
                    if (to.has_value() && found_version > to) {
                        continue;
                    }
                    if (!from.has_value() || found_version >= from) {
                        sorted_paths.insert(entry.path());
                    }
                }
            }
        }

        for (const auto &path: sorted_paths) {
            return_files.push_back(path);
        }

        return return_files;
    }

    void deeplog::read_main() {

        std::vector<std::filesystem::path> sorted_paths = list_files(deeplake::main_branch(), 0, std::nullopt);
        all_branches_ = std::make_shared<std::vector<deeplake::branch>>();
        for (const auto &path: sorted_paths) {
            std::ifstream ifs(path);

            nlohmann::json jsonArray = nlohmann::json::parse(ifs);
            for (auto &element: jsonArray) {
                if (element.contains("protocol")) {
                    auto protocol = deeplake::protocol_action(element);
                    protocol_ = std::make_shared<deeplake::protocol>(
                            deeplake::protocol(protocol.min_reader_version(), protocol.min_writer_version(), {},
                                               {}));
                } else if (element.contains("metadata")) {
                    auto metadata = deeplake::metadata_action(element);
                    metadata_ = std::make_shared<deeplake::metadata>(
                            deeplake::metadata(metadata.id(), metadata.name(), metadata.description(),
                                               metadata.created_time()));
                } else if (element.contains("createBranch")) {
                    auto action = deeplake::create_branch_action(element);
                    all_branches_->push_back(
                            deeplake::branch(action.id(), action.name(), action.from_id(), action.from_version()));
                }
            }
        }

    }

    long deeplog::file_version(const std::filesystem::path &path) const {
        auto formatted_version = path.filename().string()
                .substr(0, path.filename().string().length() - 5);
        return std::stol(formatted_version);
    }


    std::string deeplog::path() { return path_; }
} // deeplake