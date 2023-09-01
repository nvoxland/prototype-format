#include "metadata_action.hpp"
#include "nlohmann/json.hpp"

using json = nlohmann::json;

namespace deeplake {


    deeplake::metadata_action::metadata_action(std::string id,
                                               std::optional<std::string> name,
                                               std::optional<std::string> description,
                                               long created_time) :
            id_(id), name_(name), description_(description), created_time_(created_time) {}

    metadata_action::metadata_action(const nlohmann::json &j) {
        const auto &base = j.at("metadata");
        base.at("id").get_to(id_);
        if (!base.at("name").is_null()) {
            name_ = base.at("name").get<std::string>();
        }
        if (!base.at("description").is_null()) {
            description_ = base.at("description").get<std::string>();
        }

        base.at("createdTime").get_to(created_time_);
    }


    std::string deeplake::metadata_action::id() const {
        return id_;
    }

    std::optional<std::string> deeplake::metadata_action::name() const  {
        return name_;
    }

    std::optional<std::string> deeplake::metadata_action::description() const {
        return description_;
    }

    long deeplake::metadata_action::created_time()const  {
        return created_time_;
    }

    void deeplake::metadata_action::to_json(nlohmann::json &j) {
        j["metadata"]["id"] = id_;
        if (name_.has_value()) {
            j["metadata"]["name"] = name_.value();
        } else {
            j["metadata"]["name"] = json::value_t::null;
        }

        if (description_.has_value()) {
            j["metadata"]["description"] = description_.value();
        } else {
            j["metadata"]["description"] = json::value_t::null;
        }
        j["metadata"]["createdTime"] = created_time_;

    }

    std::string generate_uuid() {
        std::random_device rd;
        auto seed_data = std::array<int, std::mt19937::state_size>{};
        std::generate(std::begin(seed_data), std::end(seed_data), std::ref(rd));
        std::seed_seq seq(std::begin(seed_data), std::end(seed_data));
        std::mt19937 generator(seq);

        return uuids::to_string(uuids::uuid_random_generator{generator}());
    }

    long current_timestamp() {
        return std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::system_clock::now().time_since_epoch()).count();
    }
}