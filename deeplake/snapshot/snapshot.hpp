#ifndef DEEPLAKE_SNAPSHOT_HPP
#define DEEPLAKE_SNAPSHOT_HPP

#include <string>
#include "../dataset.hpp"
#include "scan_result.hpp"
#include "../branch.hpp"
#include "../protocol.hpp"
#include "../metadata.hpp"
#include "../transaction/optimistic_transaction.hpp"
#include "../deeplog/deeplog.hpp"

namespace deeplake {

    class dataset;

    class optimistic_transaction;

    class snapshot : public std::enable_shared_from_this<snapshot> {
    public:
        snapshot(deeplake::branch branch, long version, std::vector<deeplake::add_file_action> files,
                 std::shared_ptr<deeplog> deeplog);

        long version() const;

        deeplake::branch branch() const;

        std::unique_ptr<deeplake::snapshot> create_branch(std::string name);

        std::unique_ptr<deeplake::optimistic_transaction> start_transaction();

        scan_result scan();


    private:
        long version_;
        deeplake::branch branch_;
        std::vector<add_file_action> files_;
        std::shared_ptr<deeplake::deeplog> deeplog_;
        std::shared_ptr<deeplake::dataset> dataset_;
    };

}


#endif //DEEPLAKE_SNAPSHOT_HPP
