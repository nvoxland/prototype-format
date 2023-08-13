#include "version_control.hpp"
#include <ranges>

namespace deeplake {
    version_control::version_control(std::vector<create_branch_action> actions) : all_branches_(std::vector<deeplake::branch>()) {
        for (auto action: actions) {
            all_branches_.push_back(deeplake::branch(action.id(), action.name(), action.from_branch_id(), action.from_version()));
        }
    }

    std::vector<branch> version_control::all_branches() {
        return all_branches_;
    }

    deeplake::branch version_control::branch(const std::string branch_name) {
        auto branch = std::ranges::find_if(all_branches_, [branch_name](deeplake::branch b) { return b.name() == branch_name; });
        if (branch == all_branches_.end()) {
            throw std::runtime_error("Branch '" + branch_name + "' not found");
        }
        return *branch;
    }

    deeplake::branch version_control::branch_by_id(const std::string branch_id) {
        auto branch = std::ranges::find_if(all_branches_, [branch_id](deeplake::branch b) { return b.id() == branch_id; });
        if (branch == all_branches_.end()) {
            throw std::runtime_error("Branch id '" + branch_id + "' not found");
        }
        return *branch;
    }

}