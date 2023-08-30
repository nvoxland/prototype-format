#include "snapshot.hpp"
#include "../deeplog/actions/create_branch_action.hpp"
#include "../deeplog/actions/metadata_action.hpp"
#include <fstream>
#include <nlohmann/json.hpp>
#include <iostream>
#include <set>

using json = nlohmann::json;

namespace deeplake {

    snapshot::snapshot(deeplake::branch branch, long version, std::vector<std::string> files,
                       std::shared_ptr<deeplog> deeplog) :
            branch_(branch),
            version_(version),
            files_(files),
            deeplog_(deeplog) {

    }

    long snapshot::version() const { return version_; }

    deeplake::branch snapshot::branch() const { return branch_; }

    std::unique_ptr<optimistic_transaction> snapshot::start_transaction() {
        return std::make_unique<optimistic_transaction>(deeplake::optimistic_transaction(branch_, version_, deeplog_));
    }


    scan_result snapshot::scan() {
        return scan_result(std::vector<std::string>{});
    }

    std::unique_ptr<deeplake::snapshot> snapshot::create_branch(std::string name) {
        auto action = deeplake::create_branch_action(generate_uuid(), name, branch_.id(), version_);

        deeplog_->commit(main_branch(), deeplog_->branch_version(main_branch()), {&action});

        return deeplog_->snapshot(deeplog_->branch_by_id(action.id()), std::nullopt);
    }
}