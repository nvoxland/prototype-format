#ifndef DEEPLAKE_BRANCH_HPP
#define DEEPLAKE_BRANCH_HPP

#include <string>


namespace deeplake {
    class branch {
    public:
        branch(std::string id, std::string name, std::string from_id, long from_version);

        std::string id();

        std::string name();

        std::string from_id();

        long from_version();

        bool is_main();

    private:
        std::string id_;
        std::string name_;
        std::string from_id_;
        long from_version_;
    };

    deeplake::branch main_branch();
}



#endif //DEEPLAKE_BRANCH_HPP
