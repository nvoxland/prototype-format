#include "branches.hpp"

namespace deeplake {
    deeplake::branches::branches(std::shared_ptr<deeplake::deeplog> deeplog) : deeplog_(deeplog) {}


    std::vector<deeplake::branch>::iterator branches::begin() {
        return deeplog_->all_branches()->begin();
    }

    std::vector<deeplake::branch>::iterator branches::end() {
        return deeplog_->all_branches()->end();
    }

    unsigned long branches::size() const {
        return deeplog_->all_branches()->size();
    }

    deeplake::branch branches::branch(const std::string &branch_name) const {
        return deeplog_->branch(branch_name);
    }

    deeplake::branch branches::branch_by_id(const std::string &branch_id) const {
        return deeplog_->branch_by_id(branch_id);
    }

    long branches::branch_version(const deeplake::branch &branch) {
        return deeplog_->branch_version(branch);
    }
}
