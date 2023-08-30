#ifndef DEEPLAKE_COMMIT_RESULT_HPP
#define DEEPLAKE_COMMIT_RESULT_HPP

#include "../snapshot/snapshot.hpp"
#include <string>

namespace deeplake {

    class commit_result {
    public:
        commit_result(long version) : version_(version) {}

        long version() const;

    private:
        long version_;
    };
}


#endif //DEEPLAKE_COMMIT_RESULT_HPP
