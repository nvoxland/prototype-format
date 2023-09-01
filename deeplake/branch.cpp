#include "branch.hpp"
#include "branches.hpp"

namespace deeplake {
    branch::branch(std::string id, std::string name, std::string from_id, long from_version) :
            id_(id), name_(name), from_id_(from_id), from_version_(from_version) {}

    branch::branch(const deeplake::create_branch_action &action) :
            id_(action.id()), name_(action.name()), from_id_(action.from_id()), from_version_(action.from_version()) {}

    std::string branch::id() const { return id_; }

    std::string branch::name() const { return name_; }

    std::string branch::from_id() const { return from_id_; }

    long branch::from_version() const { return from_version_; }

    bool branch::is_main() const {
        return id_ == deeplake::branches::main().id();
    }
}