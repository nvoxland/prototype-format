#ifndef DEEPLAKE_BRANCHES_HPP
#define DEEPLAKE_BRANCHES_HPP

#include "deeplog/deeplog.hpp"

namespace deeplake {

    class deeplog;

    class branches {

    public:
        branches(std::shared_ptr<deeplake::deeplog> deeplog);

        std::vector<deeplake::branch>::iterator begin();

        std::vector<deeplake::branch>::iterator end();

        unsigned long size() const;

        deeplake::branch branch(const std::string &branch_name) const;

        deeplake::branch branch_by_id(const std::string &branch_id) const;

        long branch_version(const deeplake::branch &branch);


    private:
        std::shared_ptr <deeplake::deeplog> deeplog_;
    };
}

#endif //DEEPLAKE_BRANCHES_HPP
