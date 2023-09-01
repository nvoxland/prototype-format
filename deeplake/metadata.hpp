#ifndef DEEPLAKE_METADATA_HPP
#define DEEPLAKE_METADATA_HPP

#include <string>
#include "deeplog/actions/metadata_action.hpp"

namespace deeplake {
    class metadata {
    public:
        metadata(std::string id,
                 std::optional<std::string> name,
                 std::optional<std::string> description,
                 long created_time
        );

        metadata(const deeplake::metadata_action &action);


        std::string id();

        std::optional<std::string> name();

        std::optional<std::string> description();

        long created_time();

    private:
        std::string id_;
        std::optional<std::string> name_;
        std::optional<std::string> description_;
        long created_time_;

    };
}


#endif //DEEPLAKE_METADATA_HPP
