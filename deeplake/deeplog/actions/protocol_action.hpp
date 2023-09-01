#ifndef DEEPLAKE_PROTOCOL_ACTION_HPP
#define DEEPLAKE_PROTOCOL_ACTION_HPP

#include "action.hpp"

namespace deeplake {

    class protocol_action : public action {
    public:
        protocol_action(int min_reader_version, int min_writer_version);

        protocol_action(const nlohmann::json &j);

        int min_reader_version() const;

        int min_writer_version() const;

        virtual void to_json(nlohmann::json &json) override;


    private:
        int min_reader_version_;
        int min_writer_version_;
    };

}
#endif //DEEPLAKE_PROTOCOL_ACTION_HPP
