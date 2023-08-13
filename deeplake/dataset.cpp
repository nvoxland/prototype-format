#include <string>
#include "dataset.hpp"
#include "actions/action.hpp"
#include "actions/protocol_action.hpp"
#include "actions/metadata_action.hpp"
#include "actions/create_branch_action.hpp"
#include <filesystem>

using json = nlohmann::json;

namespace deeplake {
    dataset::dataset(const std::string path) : path_(path) {
        update();
    }


    std::string dataset::path() const {
        return path_;
    }

    std::shared_ptr<snapshot> dataset::snapshot() {
        return current_snapshot_;
    }

    dataset create_dataset(const std::string path) {
        if (std::filesystem::exists(path)) {
            throw std::runtime_error("'" + path + "' already exists");
        }

        std::filesystem::create_directories(path);
        std::filesystem::create_directories(path + "/_deeplake_log");


        auto ds = dataset(path);
        const std::unique_ptr<optimistic_transaction> tx = ds.start_transaction();

        std::vector<action *> actions;

        auto protocol = deeplake::protocol_action(4, 4);
        auto metadata = deeplake::metadata_action(generate_uuid(), std::nullopt, std::nullopt, current_timestamp());

        auto mainBranch = main_branch();
        auto branch = deeplake::create_branch_action(mainBranch.id(), mainBranch.name(), mainBranch.from_id(), mainBranch.from_version());

        actions.push_back(&protocol);
        actions.push_back(&metadata);
        actions.push_back(&branch);

        tx->commit(actions);

        return dataset(path);
    }

    std::unique_ptr<optimistic_transaction> dataset::start_transaction() {
        return std::make_unique<optimistic_transaction>(deeplake::optimistic_transaction(this));
    }

    std::shared_ptr<version_control> dataset::version_control() {
        return version_control_;
    }

    void dataset::update() {
        main_snapshot_ = std::make_shared<deeplake::snapshot>(deeplake::snapshot(main_branch(), this));
        version_control_ = std::make_shared<deeplake::version_control>(
                deeplake::version_control(main_snapshot_->branches()));

        if (current_snapshot_ == nullptr || current_snapshot_->branch().is_main()) {
            current_snapshot_ = main_snapshot_;
        } else {
            current_snapshot_ = std::make_shared<deeplake::snapshot>(deeplake::snapshot(current_snapshot_->branch(), this));
        }
    }

    std::shared_ptr<snapshot> dataset::create_branch(std::string name) {
        auto new_id = generate_uuid();

        create_branch_action action = create_branch_action(new_id, name,
                                                           current_snapshot_->branch().id(),
                                                           current_snapshot_->version() + 1);

        start_transaction()->commit(std::vector<deeplake::action *>{&action});
        return checkout_branch(name);
    }

    std::shared_ptr<snapshot> dataset::checkout_branch(std::string branch_name) {
        const auto &branch = version_control()->branch(branch_name);

        current_snapshot_ = std::make_shared<deeplake::snapshot>(deeplake::snapshot(branch, this));
        return current_snapshot_;
    }

}