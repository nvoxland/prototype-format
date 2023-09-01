#ifndef DEEPLAKE_SNAPSHOT_HPP
#define DEEPLAKE_SNAPSHOT_HPP

#include <string>
#include "scan_result.hpp"
#include "../deeplog/deeplog.hpp"

namespace deeplake {

    class snapshot {
    public:
        snapshot(std::string branch_id, long version, std::vector<deeplake::add_file_action> files);

        long version() const;

        std::string branch_id() const;

//        std::unique_ptr<deeplake::snapshot> create_branch(std::string name);

//        std::unique_ptr<deeplake::optimistic_transaction> start_transaction();
//
//        scan_result scan();


    private:
        long version_;
        std::string branch_id_;
        std::vector<add_file_action> files_;
    };

}


#endif //DEEPLAKE_SNAPSHOT_HPP
