#include "commit_result.hpp"
#include "nlohmann/json.hpp"
#include <sstream>
#include <fstream>

using json = nlohmann::json;

namespace deeplake {

    long commit_result::version() const {
        return version_;
    }
}