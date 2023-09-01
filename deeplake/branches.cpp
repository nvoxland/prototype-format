#include "branches.hpp"

namespace deeplake {
    deeplake::branches::branches(std::shared_ptr<deeplake::deeplog> deeplog) : deeplog_(deeplog) {}


    deeplake::branch branches::main() {
        return deeplake::branch("", "main", "", -1);
    }

    std::vector<std::shared_ptr<deeplake::branch>>::iterator branches::begin() {
        std::vector<std::shared_ptr<deeplake::branch>> branches;
        for (const auto& branch: *deeplog_->branches().data) {
            branches.push_back(std::make_shared<deeplake::branch>(deeplake::branch(branch)));
        }
        return std::begin(branches);
        return branches.begin();
    }

    std::vector<std::shared_ptr<deeplake::branch>>::iterator branches::end() {
        std::vector<std::shared_ptr<deeplake::branch>> branches;
        for (auto &branch: *deeplog_->branches().data) {
            branches.push_back(std::make_shared<deeplake::branch>(deeplake::branch(branch)));
        }
        return branches.end();

    }

    unsigned long branches::size() const {
        return deeplog_->branches().data->size();
    }

    std::optional<deeplake::branch> branches::branch(const std::string &branch_name) const {
        for (auto &branch: *deeplog_->branches().data) {
            if (branch.name() == branch_name) {
                return deeplake::branch(branch);
            }
        }
        return std::nullopt;
    }

    std::optional<deeplake::branch> branches::branch_by_id(const std::string &branch_id) const {
        for (auto &branch: *deeplog_->branches().data) {
            if (branch.id() == branch_id) {
                return deeplake::branch(branch);
            }
        }
        return std::nullopt;
    }

    std::optional<long> branches::version(const deeplake::branch &branch) const {
        for (auto &found_branch: *deeplog_->branches().data) {
            if (branch.id() == found_branch.id()) {
                return deeplog_->version(branch.id());
            }
        }
        return std::nullopt;
    }
}
