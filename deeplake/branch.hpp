#ifndef DEEPLAKE_BRANCH_HPP
#define DEEPLAKE_BRANCH_HPP

#include <string>
#include "deeplog/actions/create_branch_action.hpp"


namespace deeplake {
    class branch {
    public:
        branch(std::string id, std::string name, std::string from_id, long from_version);

        branch(const deeplake::create_branch_action &action);

        std::string id() const;

        std::string name() const;

        std::string from_id() const;

        long from_version() const;

        bool is_main() const;

    private:
        std::string id_;
        std::string name_;
        std::string from_id_;
        long from_version_;
    };
}



#endif //DEEPLAKE_BRANCH_HPP
