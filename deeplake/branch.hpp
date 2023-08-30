#ifndef DEEPLAKE_BRANCH_HPP
#define DEEPLAKE_BRANCH_HPP

#include <string>


namespace deeplake {
    class branch {
    public:
        branch(std::string id, std::string name, std::string from_id, long from_version);

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

    deeplake::branch main_branch();
}



#endif //DEEPLAKE_BRANCH_HPP
