#include "deeplog.hpp"
#include <filesystem>
#include <iostream>
#include <set>
#include <fstream>
#include "nlohmann/json.hpp"
#include "actions/protocol_action.hpp"
#include "actions/metadata_action.hpp"
#include "actions/create_branch_action.hpp"
#include "arrow/io/file.h"
#include "parquet/stream_writer.h"
#include "parquet/arrow/writer.h"
#include "arrow/util/type_fwd.h"
#include "arrow/table.h"
#include "arrow/array.h"
#include "arrow/builder.h"
#include "arrow/api.h"
#include "last_checkpoint.hpp"

namespace deeplake {

    deeplog::deeplog(std::string path) : path_(path) {};

    std::shared_ptr<deeplake::deeplog> deeplog::create(std::string path) {
        if (std::filesystem::exists(path)) {
            throw std::runtime_error("'" + path + "' already exists");
        }

        std::filesystem::create_directories(path);
        std::filesystem::create_directories(path + "/_deeplake_log");

        auto log = open(path);
        std::vector<action *> actions;

        auto protocol = deeplake::protocol_action(4, 4);
        auto metadata = deeplake::metadata_action(generate_uuid(), std::nullopt, std::nullopt, current_timestamp());

        auto branch = deeplake::create_branch_action(MAIN_BRANCH_ID, "main", MAIN_BRANCH_ID, -1);

        log->commit(MAIN_BRANCH_ID, -1, {&protocol, &metadata, &branch});

        return log;

    }

    std::shared_ptr<deeplake::deeplog> deeplog::open(std::string path) {
        const deeplake::deeplog &log = deeplog(path);
        return std::make_shared<deeplake::deeplog>(log);
    }

    std::string deeplog::path() { return path_; }

    long deeplog::version() const {
        return version(MAIN_BRANCH_ID);
    }

    long deeplog::version(const std::string &branch_id) const {
        return list_files(branch_id, 0, std::nullopt).version;
    }

    deeplog_state<std::shared_ptr<deeplake::protocol_action>> deeplog::protocol() const {
        auto tx_files = list_files(MAIN_BRANCH_ID, 0, std::nullopt);

        std::shared_ptr<protocol_action> protocol;

        for (const auto &path: tx_files.data) {
            std::ifstream ifs(path);

            nlohmann::json jsonArray = nlohmann::json::parse(ifs);
            for (auto &element: jsonArray) {
                if (element.contains("protocol")) {
                    auto parsed = deeplake::protocol_action(element);
                    protocol = std::make_shared<protocol_action>(parsed);
                }
            }
        }

        return {protocol, tx_files.version};
    }

    deeplog_state<std::shared_ptr<deeplake::metadata_action>> deeplog::metadata() const {
        auto tx_files = list_files(MAIN_BRANCH_ID, 0, std::nullopt);

        std::shared_ptr<metadata_action> metadata;

        for (const auto &path: tx_files.data) {
            std::ifstream ifs(path);

            nlohmann::json jsonArray = nlohmann::json::parse(ifs);
            for (auto &element: jsonArray) {
                if (element.contains("metadata")) {
                    auto parsed = deeplake::metadata_action(element);
                    metadata = std::make_shared<metadata_action>(parsed);
                }
            }
        }

        return {metadata, tx_files.version};
    }

    deeplog_state<std::vector<deeplake::add_file_action>> deeplog::data_files(const std::string &branch_id, const std::optional<long> &version) {
        auto tx_files = list_files(branch_id, 0, version);

        std::vector<add_file_action> data_files = {};

        for (const auto &path: tx_files.data) {
            std::ifstream ifs(path);

            nlohmann::json jsonArray = nlohmann::json::parse(ifs);
            for (auto &element: jsonArray) {
                if (element.contains("add")) {
                    auto add = deeplake::add_file_action(element);
                    data_files.push_back(add);
                }
            }
        }

        return {std::move(data_files), tx_files.version};
    }

    deeplog_state<std::shared_ptr<std::vector<deeplake::create_branch_action>>> deeplog::branches() const {
        auto tx_files = list_files(MAIN_BRANCH_ID, 0, std::nullopt);

        std::vector<create_branch_action> branches = {};

        for (const auto &path: tx_files.data) {
            std::ifstream ifs(path);

            nlohmann::json jsonArray = nlohmann::json::parse(ifs);
            for (auto &element: jsonArray) {
                if (element.contains("createBranch")) {
                    auto parsed = deeplake::create_branch_action(element);
                    branches.push_back(parsed);
                }
            }
        }

        return {std::make_shared<std::vector<create_branch_action>>(branches), tx_files.version};
    }


    void deeplog::commit(const std::string &branch_id,
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

        auto log_dir = path_ + "/_deeplake_log/" + branch_id + "/";

        std::filesystem::create_directories(log_dir);

        auto operationFilePath = log_dir + ss.str() + ".json";

        std::fstream file(operationFilePath, std::ios::out);
        if (!file.is_open()) {
            throw std::runtime_error("Error opening file: " + operationFilePath);
        }

        file << commit_json;
        file.close();
    }

    deeplog_state<std::shared_ptr<deeplake::create_branch_action>> deeplog::branch_by_id(const std::string &branch_id) const {
        auto all_branches = this->branches();
        auto data = all_branches.data;

        auto branch = std::ranges::find_if(*data,
                                           [branch_id](deeplake::create_branch_action b) { return b.id() == branch_id; });
        if (branch == data->end()) {
            throw std::runtime_error("Branch id '" + branch_id + "' not found");
        }

        return {std::make_shared<deeplake::create_branch_action>(*branch), all_branches.version};
    }

//    long deeplake::deeplog::branch_version(const deeplake::branch &branch) {
//        return list_files(branch, 0, std::nullopt).version;
//    }

    deeplog_state<std::vector<std::filesystem::path>> deeplog::list_files(const std::string &branch_id,
                                                                          const std::optional<long> &from,
                                                                          const std::optional<long> &to) const {
        std::optional<long> next_from = from;
        std::vector<std::filesystem::path> return_files = {};

        if (branch_id != MAIN_BRANCH_ID) {
            auto branch_obj = branch_by_id(branch_id).data;
            for (const auto &file: list_files(branch_id, from, branch_obj->from_version()).data) {
                return_files.push_back(file);
            }

            next_from = branch_obj->from_version() + 1;
        }

        std::set < std::filesystem::path > sorted_paths = {};

        long higheset_version = -1;
        std::filesystem::path dir_path = {path_ + "/_deeplake_log/" + branch_id};
        if (std::filesystem::exists(dir_path)) {
            for (const auto &entry: std::filesystem::directory_iterator(dir_path)) {
                if (std::filesystem::is_regular_file(entry.path()) && entry.path().extension() == ".json") {
                    auto found_version = file_version(entry.path());
                    if (higheset_version < found_version) {
                        higheset_version = found_version;
                    }
                    if (to.has_value() && found_version > to) {
                        continue;
                    }
                    if (!next_from.has_value() || found_version >= next_from) {
                        sorted_paths.insert(entry.path());
                    }
                }
            }
        }

        for (const auto &path: sorted_paths) {
            return_files.push_back(path);
        }

        return deeplog_state(return_files, higheset_version);
    }

    long deeplog::file_version(const std::filesystem::path &path) const {
        auto formatted_version = path.filename().string()
                .substr(0, path.filename().string().length() - 5);
        return std::stol(formatted_version);
    }

    void deeplog::checkpoint() {
        auto status = write_checkpoint();

        if (!status.ok()) {
            return;
        }
        nlohmann::json checkpoint_json = last_checkpoint(21, 3013);

        auto checkpoint_path = path_ + "/_deeplake_log/_last_checkpoint.json";
        std::fstream file(checkpoint_path, std::ios::out);
        if (!file.is_open()) {
            throw std::runtime_error("Error opening file: " + checkpoint_path);
        }

        file << checkpoint_json;
        file.close();
    }

    arrow::Status deeplog::write_checkpoint() {
        auto protocol_builder = deeplake::protocol_action::arrow_array();
        auto metadata_builder = deeplake::metadata_action::arrow_array();

        ARROW_RETURN_NOT_OK(protocol().data->append(protocol_builder));
        ARROW_RETURN_NOT_OK(metadata_builder->AppendNull());

        ARROW_RETURN_NOT_OK(protocol_builder->AppendNull());
        ARROW_RETURN_NOT_OK(metadata().data->append(metadata_builder));

        std::shared_ptr<arrow::Array> protocol_array, metadata_array;
        ARROW_RETURN_NOT_OK(protocol_builder->Finish(&protocol_array));
        ARROW_RETURN_NOT_OK(metadata_builder->Finish(&metadata_array));

        auto schema = std::make_shared<arrow::Schema>(arrow::FieldVector{
                arrow::field("protocol", protocol_builder->type()),
                arrow::field("metadata", metadata_builder->type()),
        });
        const auto protocol_table = arrow::Table::Make(schema, {protocol_array, metadata_array});

        std::shared_ptr<parquet::WriterProperties> props = parquet::WriterProperties::Builder().compression(arrow::Compression::SNAPPY)->build();
        std::shared_ptr<parquet::ArrowWriterProperties> arrow_props = parquet::ArrowWriterProperties::Builder().store_schema()->build();

        std::shared_ptr<arrow::io::FileOutputStream> outfile;
        ARROW_ASSIGN_OR_RAISE(outfile, arrow::io::FileOutputStream::Open(path_ + "/_deeplake_log/00000000000000000021.checkpoint.parquet"));
//
        ARROW_RETURN_NOT_OK(parquet::arrow::WriteTable(*protocol_table, arrow::default_memory_pool(), outfile, 3, props, arrow_props));

        return arrow::Status::OK();
    }

} // deeplake