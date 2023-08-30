#include "protocol_action.hpp"

namespace deeplake {
    deeplake::protocol_action::protocol_action(int min_reader_version, int min_writer_version)
            : min_reader_version_(min_reader_version), min_writer_version_(min_writer_version) {}

    protocol_action::protocol_action(const nlohmann::json &j) {
        j.at("protocol").at("minReaderVersion").get_to(min_reader_version_);
        j.at("protocol").at("minWriterVersion").get_to(min_writer_version_);
    }

    int protocol_action::min_reader_version() {
        return min_reader_version_;
    }

    int protocol_action::min_writer_version() {
        return min_writer_version_;
    }

    void deeplake::protocol_action::to_json(nlohmann::json &j) {
        j["protocol"]["minReaderVersion"] = min_reader_version_;
        j["protocol"]["minWriterVersion"] = min_writer_version_;
    }
}