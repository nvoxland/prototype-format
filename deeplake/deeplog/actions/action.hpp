#ifndef DEEPLAKE_ACTION_HPP
#define DEEPLAKE_ACTION_HPP

#include "nlohmann/json.hpp"

namespace deeplake {

    class action {

    public:

        virtual void to_json(nlohmann::json &json) = 0;

    };

}
#endif //DEEPLAKE_ACTION_HPP
