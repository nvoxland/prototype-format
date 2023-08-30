#ifndef DEEPLAKE_OPTIMISTIC_TRANSACTION_HPP
#define DEEPLAKE_OPTIMISTIC_TRANSACTION_HPP

#include "../snapshot/snapshot.hpp"
#include <string>
#include "commit_result.hpp"
#include "../dataset.hpp"
#include "../branch.hpp"

namespace deeplake {
    class dataset;

    class action;

    class commit_result;

    class snapshot;

    class deeplog;

    class optimistic_transaction {
    public:
        optimistic_transaction(const deeplake::branch &snapshot_branch,
                               const long &snapshot_version,
                               const std::shared_ptr<deeplake::deeplog> deeplog);

        deeplake::commit_result commit(std::vector<deeplake::action *> actions);

    private:
        deeplake::branch snapshot_branch_;
        long snapshot_version_;
        std::shared_ptr<deeplake::deeplog> deeplog_;
    };
}


#endif //DEEPLAKE_OPTIMISTIC_TRANSACTION_HPP
