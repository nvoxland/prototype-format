#ifndef DEEPLAKE_DATASET_H
#define DEEPLAKE_DATASET_H

#include <filesystem>
#include "transaction/optimistic_transaction.hpp"
#include "snapshot/snapshot.hpp"
#include "version_control/version_control.hpp"
#include <map>

namespace deeplake {

    class snapshot;

    class optimistic_transaction;

    class dataset {
    public:
        explicit dataset(const std::string path);

        std::string path() const;

        std::shared_ptr<snapshot> snapshot();

        std::unique_ptr<optimistic_transaction> start_transaction();

        std::shared_ptr<version_control> version_control();

        void update();

        std::shared_ptr<deeplake::snapshot> create_branch(std::string name);
        std::shared_ptr<deeplake::snapshot> checkout_branch(std::string branch_name);

    private:
        std::string path_;
        std::shared_ptr<deeplake::snapshot> main_snapshot_;
        std::shared_ptr<deeplake::snapshot> current_snapshot_;
        std::shared_ptr<deeplake::version_control> version_control_;
    };

    dataset create_dataset(const std::string path);

}

#endif //DEEPLAKE_DATASET_H