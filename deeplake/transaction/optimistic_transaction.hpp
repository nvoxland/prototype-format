#ifndef DEEPLAKE_OPTIMISTIC_TRANSACTION_HPP
#define DEEPLAKE_OPTIMISTIC_TRANSACTION_HPP

#include "../snapshot/snapshot.hpp"
#include "../actions/action.hpp"
#include <string>
#include "commit_result.hpp"
#include "../dataset.hpp"

namespace deeplake {
    class dataset;

    class action;

    class commit_result;

    class optimistic_transaction {
    public:
        optimistic_transaction(deeplake::dataset *dataset);

        deeplake::commit_result commit(std::vector<deeplake::action *> actions);

    private:
        deeplake::dataset *dataset_;
    };
}


#endif //DEEPLAKE_OPTIMISTIC_TRANSACTION_HPP
