#include "snapshot.hpp"
#include <nlohmann/json.hpp>
#include <iostream>
#include <set>

using json = nlohmann::json;

namespace deeplake {

    snapshot::snapshot(std::string branch_id, long version, std::vector<add_file_action> files) :
            branch_id_(branch_id),
            version_(version),
            files_(files) {

    }

    long snapshot::version() const { return version_; }

    std::string snapshot::branch_id() const { return branch_id_; }

//    std::unique_ptr<optimistic_transaction> snapshot::start_transaction() {
//        return std::make_unique<optimistic_transaction>(deeplake::optimistic_transaction(branch_, version_, deeplog_));
//    }
//
//
//    scan_result snapshot::scan() {
//        return scan_result(std::vector<std::string>{});
//    }

//    std::unique_ptr<deeplake::snapshot> snapshot::create_branch(std::string name) {
//        auto action = deeplake::create_branch_action(generate_uuid(), name, branch_.id(), version_);
//
//        deeplog_->commit(branch::main().id(), deeplog_->version(branch::main().id()), {&action});
//
//        return dataset_->snapshot(deeplog_->branch_by_id(action.id()).data->id());
//    }
}