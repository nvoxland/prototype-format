#include "optimistic_transaction.hpp"
#include "nlohmann/json.hpp"
#include <sstream>
#include <fstream>

using json = nlohmann::json;

namespace deeplake {

    optimistic_transaction::optimistic_transaction(deeplake::dataset *dataset) : dataset_(dataset) {}

    commit_result optimistic_transaction::commit(std::vector<deeplake::action *> actions) {
        long nextVersion = dataset_->snapshot()->version() + 1;

        json commit_json;

        for (auto action: actions) {
            json action_json = json::object();
            action->to_json(action_json);
            commit_json.push_back(action_json);
        }

        std::ostringstream ss;
        ss << std::setw(20) << std::setfill('0') << nextVersion;

        auto branch = dataset_->snapshot()->branch();
        
        auto log_dir = dataset_->path() + "/_deeplake_log/" + branch.id() + "/";

        std::filesystem::create_directories(log_dir);

        auto operationFilePath = log_dir + ss.str() + ".json";

        std::fstream file(operationFilePath, std::ios::out);
        if (!file.is_open()) {
            throw std::runtime_error("Error opening file: " + operationFilePath);
        }

        file << commit_json;
        file.close();

        dataset_->update();

        return commit_result(nextVersion);
    }
}