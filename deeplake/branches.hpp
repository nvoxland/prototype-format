#ifndef DEEPLAKE_BRANCHES_HPP
#define DEEPLAKE_BRANCHES_HPP

#include "deeplog/deeplog.hpp"

namespace deeplake {

    class deeplog;

    class branches {

    public:
        static branch main();

        branches(std::shared_ptr<deeplake::deeplog> deeplog);

        std::vector<std::shared_ptr<deeplake::branch>>::iterator begin();

        std::vector<std::shared_ptr<deeplake::branch>>::iterator end();

        unsigned long size() const;

        std::optional<deeplake::branch> branch(const std::string &branch_name) const;

        std::optional<deeplake::branch> branch_by_id(const std::string &branch_id) const;

        std::optional<long> version(const deeplake::branch &branch) const;


    private:
        std::shared_ptr <deeplake::deeplog> deeplog_;
    };
}

#endif //DEEPLAKE_BRANCHES_HPP
