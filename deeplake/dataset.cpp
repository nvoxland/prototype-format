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
        return deeplog_->snapshot(branch, std::nullopt);
    }

    std::unique_ptr<deeplake::snapshot> dataset::snapshot(const std::string &branch_name) {
        return this->snapshot(branches_->branch(branch_name));
    }

    std::unique_ptr<deeplake::snapshot> dataset::snapshot() {
        return deeplog_->snapshot(main_branch(), std::nullopt);
    }

    std::unique_ptr<deeplake::snapshot> dataset::snapshot(const deeplake::branch &branch, const long &version) {
        return deeplog_->snapshot(branch, version);
    }

    std::unique_ptr<deeplake::snapshot> dataset::snapshot(const deeplake::snapshot &snapshot) {
        return this->snapshot(snapshot.branch());
    }


    std::shared_ptr<protocol> dataset::protocol() {
        return deeplog_->protocol();
    }

    std::shared_ptr<metadata> dataset::metadata() {
        return deeplog_->metadata();
    }

    std::shared_ptr<deeplake::branches> dataset::branches() {
        return branches_;
    }

    std::unique_ptr<dataset> dataset::create(const std::string &path) {
        const auto &deeplog = deeplake::deeplog::create(path);

        std::vector<action *> actions;

        auto protocol = deeplake::protocol_action(4, 4);
        auto metadata = deeplake::metadata_action(generate_uuid(), std::nullopt, std::nullopt, current_timestamp());

        auto mainBranch = main_branch();
        auto branch = deeplake::create_branch_action(mainBranch.id(), mainBranch.name(), mainBranch.from_id(),
                                                     mainBranch.from_version());

        deeplog->commit(main_branch(), -1, {&protocol, &metadata, &branch});

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