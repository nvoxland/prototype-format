#include "optimistic_transaction.hpp"
#include "nlohmann/json.hpp"
#include <sstream>
#include <fstream>

using json = nlohmann::json;

namespace deeplake {

    optimistic_transaction::optimistic_transaction(const deeplake::branch &snapshot_branch,
                                                   const long &snapshot_version,
                                                   std::shared_ptr<deeplake::deeplog> deeplog)
            : snapshot_branch_(snapshot_branch), snapshot_version_(snapshot_version), deeplog_(deeplog) {}

    commit_result optimistic_transaction::commit(std::vector<deeplake::action *> actions) {
        deeplog_->commit(snapshot_branch_.id(), snapshot_version_, actions);

//        long nextVersion = snapshot_->version() + 1;
//
//        json commit_json;
//
//        for (auto action: actions) {
//            json action_json = json::object();
//            action->to_json(action_json);
//            commit_json.push_back(action_json);
//        }
//
//        std::ostringstream ss;
//        ss << std::setw(20) << std::setfill('0') << nextVersion;
//
//        auto branch = snapshot_->branch();
//
//        auto log_dir = snapshot_-> + "/_deeplake_log/" + branch.id() + "/";
//
//        std::filesystem::create_directories(log_dir);
//
//        auto operationFilePath = log_dir + ss.str() + ".json";
//
//        std::fstream file(operationFilePath, std::ios::out);
//        if (!file.is_open()) {
//            throw std::runtime_error("Error opening file: " + operationFilePath);
//        }
//
//        file << commit_json;
//        file.close();
//
////        dataset_->update();

        return commit_result(snapshot_version_ + 1);
    }
}