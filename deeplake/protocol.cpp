#include "protocol.hpp"

namespace deeplake {
    protocol::protocol(const int& min_reader_version, const int& min_writer_version,
                                 const std::vector<std::string>& reader_features,
                                 const std::vector<std::string>& writer_features) :
            min_reader_version_(min_reader_version),
            min_writer_version_(min_writer_version),
            reader_features_(reader_features),
            writer_features_(writer_features) {}

    protocol::protocol(const deeplake::protocol_action &action) :
        min_reader_version_(action.min_reader_version()),
        min_writer_version_(action.min_writer_version())
    { }


    int deeplake::protocol::min_reader_version() {
        return min_reader_version_;
    }

    int deeplake::protocol::min_writer_version() {
        return min_writer_version_;
    }

    std::vector<std::string> deeplake::protocol::reader_features() {
        return reader_features_;
    }

    std::vector<std::string> deeplake::protocol::writer_features() {
        return writer_features_;
    }
}
