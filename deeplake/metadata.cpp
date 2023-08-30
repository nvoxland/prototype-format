#include "metadata.hpp"

namespace deeplake {
    deeplake::metadata::metadata(std::string id,
                                 std::optional<std::string> name,
                                 std::optional<std::string> description,
                                 long created_time
    ) : id_(id), name_(name), description_(description), created_time_(created_time) {}

    std::string deeplake::metadata::id() {
        return id_;
    }

    std::optional<std::string> deeplake::metadata::name() {
        return name_;
    }

    std::optional<std::string> deeplake::metadata::description() {
        return description_;
    }

    long deeplake::metadata::created_time() {
        return created_time_;
    }
}