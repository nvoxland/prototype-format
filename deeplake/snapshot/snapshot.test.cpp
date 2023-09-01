#include <gtest/gtest.h>
#include <filesystem>
#include "snapshot.hpp"

class SnapshotTest : public ::testing::Test {
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

    std::string test_dir = "tmp/test";
};

//TEST_F(SnapshotTest, create_branch) {
//    auto dataset = deeplake::dataset::create(test_dir);
//
//    EXPECT_EQ(1, dataset->branches()->size());
//
//    auto branch1_snapshot = dataset->snapshot()->create_branch("branch1");
//    EXPECT_EQ("branch1", branch1_snapshot->branch().name());
//    EXPECT_EQ(0, branch1_snapshot->branch().from_version());
//    EXPECT_EQ(deeplake::branch::main().id(), branch1_snapshot->branch().from_id());
//
//    EXPECT_EQ(1, dataset->snapshot()->version());
//    EXPECT_EQ(2, dataset->branches()->size());
//
//}
