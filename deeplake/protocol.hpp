#ifndef DEEPLAKE_PROTOCOL_HPP
#define DEEPLAKE_PROTOCOL_HPP

#include <string>
#include "deeplog/actions/protocol_action.hpp"

namespace deeplake {

    class protocol {

    public:
        protocol(const int& min_reader_version,
                 const int& min_writer_version,
                 const std::vector<std::string>& reader_features,
                 const std::vector<std::string>& writer_features);

        protocol(const deeplake::protocol_action &action);

        int min_reader_version();

        int min_writer_version();

        std::vector<std::string> reader_features();

        std::vector<std::string> writer_features();

    private:
        int min_reader_version_;
        int min_writer_version_;
        std::vector<std::string> reader_features_;
        std::vector<std::string> writer_features_;
    };
}


#endif //DEEPLAKE_PROTOCOL_HPP
