#include <string>
#include "dataset.hpp"
#include "deeplog/deeplog.hpp"
#include "deeplog/actions/protocol_action.hpp"
#include "deeplog/actions/metadata_action.hpp"
#include "deeplog/actions/create_branch_action.hpp"

using json = nlohmann::json;

namespace deeplake {

    dataset::dataset(const std::string &path, const std::shared_ptr<deeplake::deeplog> &deeplog)
            : path_(path), deeplog_(deeplog), branches_(std::make_shared<deeplake::branches>(deeplog)) {
    }


    std::string dataset::path() const {
        return path_;
    }

    std::unique_ptr<deeplake::snapshot> dataset::snapshot(const deeplake::branch &branch) {
        return snapshot(branch, std::nullopt);
    }

    std::unique_ptr<deeplake::snapshot> dataset::snapshot(const std::string &branch_name) {
        return snapshot(branches_->branch(branch_name)->id());
    }

    std::unique_ptr<deeplake::snapshot> dataset::snapshot() {
        return snapshot(branches::main(), std::nullopt);
    }

    std::unique_ptr<deeplake::snapshot> dataset::snapshot(const deeplake::branch &branch, const std::optional<long> &version) {
        auto files = deeplog_->data_files(branch.id(), version);
        return std::make_unique<deeplake::snapshot>(deeplake::snapshot(branch, files.version, files.data, deeplog_));
    }

    std::unique_ptr<deeplake::snapshot> dataset::snapshot(const deeplake::snapshot &snapshot) {
        return this->snapshot(snapshot.branch());
    }


    std::shared_ptr<deeplake::protocol> dataset::protocol() {
        return std::make_shared<deeplake::protocol>(*(deeplog_->protocol().data));
    }

    std::shared_ptr<metadata> dataset::metadata() {
        return std::make_shared<deeplake::metadata>(*(deeplog_->metadata().data));
    }

    std::shared_ptr<deeplake::branches> dataset::branches() {
        return branches_;
    }

    std::unique_ptr<dataset> dataset::create(const std::string &path) {
        const auto &deeplog = deeplake::deeplog::create(path);

        return std::make_unique<dataset>(dataset(path, deeplog));
    }

//    void dataset::add_data(std::vector<std::string> data) {
//        std::filesystem::path filename = this->path_ + "/" + generate_uuid() + ".txt";
//        auto a_stream = std::ofstream(filename);
//        for (auto &item: data) {
//            a_stream << item << std::endl;
//        }
//        a_stream.close();
//
//        auto tx = start_transaction();
//        auto action = deeplake::add_file_action(filename.filename(), std::filesystem::file_size(filename),
//                                                current_timestamp(), true);
//        tx->commit(std::vector<deeplake::action *>{&action});
//    }


}