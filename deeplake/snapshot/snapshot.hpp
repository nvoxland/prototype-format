#ifndef DEEPLAKE_SNAPSHOT_HPP
#define DEEPLAKE_SNAPSHOT_HPP

#include <string>
#include "../dataset.hpp"
#include "../actions/metadata_action.hpp"
#include "../actions/protocol_action.hpp"
#include "../actions/add_file_action.hpp"
#include "scan_result.hpp"
#include "../actions/create_branch_action.hpp"
#include "../version_control/branch.hpp"

namespace deeplake {

    class dataset;

    class snapshot {
    public:
        snapshot(deeplake::branch branch, dataset *dataset);

        std::string path();

        long version();

        deeplake::branch branch();

        protocol_action protocol();

        metadata_action metadata();

        std::vector<add_file_action> files();

        std::vector<create_branch_action> branches();

        scan_result scan();


    private:
        void replay_log(deeplake::branch branch, long from, long to, dataset *dataset);

        long version_;
        deeplake::branch branch_;
        protocol_action protocol_;
        metadata_action metadata_;
        std::vector<add_file_action> files_;
        std::vector<create_branch_action> branches_;
        dataset *dataset_;
    };


}


#endif //DEEPLAKE_SNAPSHOT_HPP
