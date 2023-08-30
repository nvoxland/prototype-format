#include <gtest/gtest.h>
#include "optimistic_transaction.hpp"
#include "../dataset.hpp"
#include "../deeplog/actions/action.hpp"
#include "../deeplog/actions/protocol_action.hpp"
#include <fstream>

class TransactionTest : public ::testing::Test {
protected:
    void SetUp() override {
        if (std::filesystem::exists(test_dir)) {
            std::filesystem::remove_all(test_dir);
        }
    }

    void TearDown() override {
        if (std::filesystem::exists(test_dir)) {
            std::filesystem::remove_all(test_dir);
        }
    }

    std::filesystem::path test_dir = "tmp/test";
};

deeplake::commit_result
write_data(const std::string &test_dir, const std::string &file_name, const std::string &content, const std::unique_ptr<deeplake::snapshot> &snapshot) {
    auto a_stream = std::ofstream(test_dir + "/" + file_name);
    a_stream << content;
    a_stream.close();

    auto tx = snapshot->start_transaction();
    auto action = deeplake::add_file_action(file_name, content.size(), 100, true);
    return tx->commit(std::vector<deeplake::action *>{&action});
}

std::string read_data(const std::unique_ptr<deeplake::snapshot> &snapshot) {
    std::stringstream file_content;
    for (auto val: snapshot->scan()) {
        file_content << val << ",";
    }

    return file_content.str();
}

TEST_F(TransactionTest, commit) {
    auto dataset = deeplake::dataset::create(test_dir);
    EXPECT_EQ(0, dataset->snapshot()->version());

    auto tx = dataset->snapshot()->start_transaction();
    auto action = deeplake::protocol_action(5, 5);
    auto action2 = deeplake::protocol_action(6, 6);

    auto resp = tx->commit({&action, &action2});
    EXPECT_EQ(1, resp.version());
    EXPECT_EQ(1, dataset->snapshot()->version());
    EXPECT_EQ(6, dataset->protocol()->min_reader_version());
}

TEST_F(TransactionTest, commit_across_branches) {
    auto dataset = deeplake::dataset::create(test_dir);
    auto main_snapshot = dataset->snapshot();
    EXPECT_EQ(0, main_snapshot->version());

    auto resp = write_data(test_dir.string(), "a.txt", "Value a1", main_snapshot);
    main_snapshot = dataset->snapshot();
    EXPECT_EQ(1, resp.version());
    EXPECT_EQ(1, main_snapshot->version());
    EXPECT_EQ("Value a1,", read_data(main_snapshot));


    auto branch1_snap = dataset->snapshot()->create_branch("branch1");
    EXPECT_EQ(2, branch1_snap->version());

    resp = write_data(test_dir.string(), "b1.txt", "Value b1", branch1_snap);
    EXPECT_EQ(3, resp.version());
    EXPECT_EQ(3, dataset->snapshot()->version());
    EXPECT_EQ("Value a1,Value b1,", read_data(dataset->snapshot()));

    main_snapshot = dataset->snapshot("main");
    EXPECT_EQ(2, main_snapshot->version());
    EXPECT_EQ("Value a1,", read_data(dataset->snapshot()));

    auto branch2_snap = dataset->snapshot("branch2");
    EXPECT_EQ(3, branch2_snap->version());
    resp = write_data(test_dir.string(), "b2.txt", "Value b2", branch2_snap);
    EXPECT_EQ(4, resp.version());
    branch2_snap = dataset->snapshot("branch2");
    EXPECT_EQ(4, branch2_snap->version());
    EXPECT_EQ("Value a1,Value b2,", read_data(branch2_snap));

    main_snapshot = dataset->snapshot("main");
    EXPECT_EQ(3, main_snapshot->version());
    EXPECT_EQ("Value a1,", read_data(main_snapshot));

    branch1_snap = dataset->snapshot("branch1");
    EXPECT_EQ(3, branch1_snap->version());
    EXPECT_EQ("Value a1,Value b1,", read_data(branch1_snap));
}
