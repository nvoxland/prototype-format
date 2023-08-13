#ifndef DEEPLAKE_ADD_FILE_ACTION_HPP
#define DEEPLAKE_ADD_FILE_ACTION_HPP

#include "action.hpp"

namespace deeplake {
    class add_file_action : public action {

    public:
        add_file_action(std::string path, long size, long modification_time);

        add_file_action(const nlohmann::json &j);

        std::string path();

        long size();

        long modification_time();

        virtual void to_json(nlohmann::json &json) override;

    private:
        std::string path_;
        long size_;
        long modification_time_;
    };
}


#endif //DEEPLAKE_ADD_FILE_ACTION_HPP
