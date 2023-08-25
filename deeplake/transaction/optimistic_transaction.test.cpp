#include <gtest/gtest.h>
#include "optimistic_transaction.hpp"
#include "../dataset.hpp"
#include "../actions/action.hpp"
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
write_data(std::string test_dir, std::string file_name, std::string content, deeplake::dataset *dataset) {
    auto a_stream = std::ofstream(test_dir + "/" + file_name);
    a_stream << content;
    a_stream.close();

    auto tx = dataset->start_transaction();
    auto action = deeplake::add_file_action(file_name, content.size(), 100, true);
    return tx->commit(std::vector<deeplake::action *>{&action});
}

std::string read_data(std::shared_ptr<deeplake::snapshot> snapshot) {
    std::stringstream file_content;
    for (auto val: snapshot->scan()) {
        file_content << val << ",";
    }

    return file_content.str();
}

TEST_F(TransactionTest, commit) {
    auto dataset = deeplake::create_dataset(test_dir);
    EXPECT_EQ(0, dataset.snapshot()->version());


    auto tx = dataset.start_transaction();
    auto action = deeplake::protocol_action(5, 5);
    auto action2 = deeplake::protocol_action(6, 6);

    std::vector<deeplake::action *> actions{};
    actions.push_back(&action);
    actions.push_back(&action2);

    auto resp = tx->commit(actions);
    EXPECT_EQ(1, resp.version());
    EXPECT_EQ(1, dataset.snapshot()->version());
    EXPECT_EQ(6, dataset.snapshot()->protocol().min_reader_version());
}

TEST_F(TransactionTest, commit_across_branches) {
    auto dataset = deeplake::create_dataset(test_dir);
    EXPECT_EQ(0, dataset.snapshot()->version());

    auto resp = write_data(test_dir.string(), "a.txt", "Value a1", &dataset);
    EXPECT_EQ(1, resp.version());
    EXPECT_EQ(1, dataset.snapshot()->version());
    EXPECT_EQ("Value a1,", read_data(dataset.snapshot()));


    dataset.create_branch("branch1");
    EXPECT_EQ(2, dataset.snapshot()->version());

    resp = write_data(test_dir.string(), "b1.txt", "Value b1", &dataset);
    EXPECT_EQ(3, resp.version());
    EXPECT_EQ(3, dataset.snapshot()->version());
    EXPECT_EQ("Value a1,Value b1,", read_data(dataset.snapshot()));

    dataset.checkout_branch("main");
    EXPECT_EQ(2, dataset.snapshot()->version());
    EXPECT_EQ("Value a1,", read_data(dataset.snapshot()));

    dataset.create_branch("branch2");
    EXPECT_EQ(3, dataset.snapshot()->version());
    resp = write_data(test_dir.string(), "b2.txt", "Value b2", &dataset);
    EXPECT_EQ(4, resp.version());
    EXPECT_EQ(4, dataset.snapshot()->version());
    EXPECT_EQ("Value a1,Value b2,", read_data(dataset.snapshot()));

    dataset.checkout_branch("main");
    EXPECT_EQ(3, dataset.snapshot()->version());
    EXPECT_EQ("Value a1,", read_data(dataset.snapshot()));

    dataset.checkout_branch("branch1");
    EXPECT_EQ(3, dataset.snapshot()->version());
    EXPECT_EQ("Value a1,Value b1,", read_data(dataset.snapshot()));
}
