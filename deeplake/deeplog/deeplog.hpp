#ifndef DEEPLAKE_DEEPLOG_HPP
#define DEEPLAKE_DEEPLOG_HPP

#include <string>
#include "../branch.hpp"
#include "actions/add_file_action.hpp"
#include "../snapshot/snapshot.hpp"
#include "../protocol.hpp"
#include "../metadata.hpp"

namespace deeplake {

    class snapshot;

    class deeplog : public std::enable_shared_from_this<deeplog> {
    public:
        static std::shared_ptr<deeplog> create(std::string path);

        static std::shared_ptr<deeplog> open(std::string path);

        std::string path();

        std::shared_ptr<deeplake::protocol> protocol() const;

        std::shared_ptr<deeplake::metadata> metadata() const;

        std::unique_ptr<deeplake::snapshot> snapshot(const deeplake::branch &branch, const std::optional<long> &version);

        void commit(const deeplake::branch &branch,
                    const long &base_version,
                    const std::vector<deeplake::action *> &actions);

        std::shared_ptr<std::vector<branch>> all_branches() const;

        deeplake::branch branch(const std::string branch_name) const;

        deeplake::branch branch_by_id(const std::string branch_id) const;

        long branch_version(const deeplake::branch &branch);

    private:

        //only created through open() etc.
        deeplog(std::string path);

        std::vector<std::filesystem::path>
        list_files(deeplake::branch branch, std::optional<long> from, std::optional<long> to);

        void read_main();

        long file_version(const std::filesystem::path &path) const;

        std::string path_;

        std::shared_ptr<deeplake::protocol> protocol_;
        std::shared_ptr<deeplake::metadata> metadata_;

        std::shared_ptr<std::vector<deeplake::branch>> all_branches_;

    };

} // deeplake

#endif //DEEPLAKE_DEEPLOG_HPP
