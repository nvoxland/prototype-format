#ifndef DEEPLAKE_DATASET_H
#define DEEPLAKE_DATASET_H

#include <filesystem>
#include "transaction/optimistic_transaction.hpp"
#include "snapshot/snapshot.hpp"
#include "deeplog/deeplog.hpp"
#include "protocol.hpp"
#include "metadata.hpp"
#include "branches.hpp"
#include <map>

namespace deeplake {

    class snapshot;

    class optimistic_transaction;

    class branches;

    class deeplog;

    class dataset : public std::enable_shared_from_this<dataset> {
    public:
        static std::unique_ptr<dataset> create(const std::string &path);

        static std::unique_ptr<dataset> open(const std::string &path);

        std::string path() const;

        std::unique_ptr<deeplake::snapshot> snapshot(const deeplake::branch &branch, const std::optional<long> &version);

        std::unique_ptr<deeplake::snapshot> snapshot(const deeplake::branch &branch);

        std::unique_ptr<deeplake::snapshot> snapshot(const std::string &branch_name);

        std::unique_ptr<deeplake::snapshot> snapshot();

        std::unique_ptr<deeplake::snapshot> snapshot(const deeplake::snapshot &snapshot);

        std::shared_ptr<protocol> protocol();

        std::shared_ptr<metadata> metadata();

//        void add_data(std::vector<std::string> data);

        std::shared_ptr<branches> branches();

    private:
        dataset(const std::string &path, const std::shared_ptr<deeplake::deeplog> &deeplog);

        std::string path_;
        std::shared_ptr<deeplake::deeplog> deeplog_;
        std::shared_ptr<deeplake::branches> branches_;
    };


}

#endif //DEEPLAKE_DATASET_H