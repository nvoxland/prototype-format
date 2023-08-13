#ifndef DEEPLAKE_VERSION_CONTROL_HPP
#define DEEPLAKE_VERSION_CONTROL_HPP

#include <string>
#include <map>
#include "../actions/create_branch_action.hpp"
#include "branch.hpp"

namespace deeplake {

    class version_control {
    public:
        version_control(std::vector<create_branch_action> actions);

        std::vector<branch> all_branches();

        deeplake::branch branch(const std::string branch_name);

        deeplake::branch branch_by_id(const std::string branch_id);

    private:
        std::vector<deeplake::branch> all_branches_;
    };
}


#endif //DEEPLAKE_VERSION_CONTROL_HPP
