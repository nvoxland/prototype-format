#ifndef DEEPLAKE_METADATA_HPP
#define DEEPLAKE_METADATA_HPP

#include <string>

namespace deeplake {
    class metadata {
    public:
        metadata(std::string id,
                 std::optional<std::string> name,
                 std::optional<std::string> description,
                 long created_time
        );

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
