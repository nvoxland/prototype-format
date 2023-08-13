#include "branch.hpp"

namespace deeplake {
    branch::branch(std::string id, std::string name, std::string from_id, long from_version) :
            id_(id), name_(name), from_id_(from_id), from_version_(from_version) {}


    std::string branch::id() { return id_; }

    std::string branch::name() { return name_; }

    std::string branch::from_id() { return from_id_; }

    long branch::from_version() { return from_version_; }

    bool branch::is_main() {
        return id_ == main_branch().id();
    }

    deeplake::branch main_branch() {
        return deeplake::branch("", "main", "", -1);
    }
}