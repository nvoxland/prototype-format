#include "create_branch_action.hpp"

namespace deeplake {
    create_branch_action::create_branch_action(std::string id,
                                               std::string name,
                                               std::string from_branch,
                                               long from_version) :
            id_(id), name_(name), from_branch_id_(from_branch), from_version_(from_version) {}

    create_branch_action::create_branch_action(const nlohmann::json &j) {
        const auto &base = j.at("createBranch");
        base.at("id").get_to(id_);
        base.at("name").get_to(name_);
        base.at("fromBranchId").get_to(from_branch_id_);
        base.at("fromVersion").get_to(from_version_);
    }

    void create_branch_action::to_json(nlohmann::json &j) {
        j["createBranch"]["id"] = id_;
        j["createBranch"]["name"] = name_;
        j["createBranch"]["fromBranchId"] = from_branch_id_;
        j["createBranch"]["fromVersion"] = from_version_;
    }

    std::string create_branch_action::id() { return id_; }

    std::string create_branch_action::name() { return name_; }

    std::string create_branch_action::from_branch_id() { return from_branch_id_; }

    long create_branch_action::from_version() { return from_version_; }
}