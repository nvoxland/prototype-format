#include <gtest/gtest.h>
#include <filesystem>
#include <fstream>
#include "dataset.hpp"
#include <nlohmann/json.hpp>

using json = nlohmann::json;

class DatasetTest : public ::testing::Test {
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

TEST_F(DatasetTest, open) {
    deeplake::create_dataset(test_dir);

    auto dataset = deeplake::dataset(test_dir);
    EXPECT_EQ(test_dir, dataset.path());
    EXPECT_EQ(0, dataset.snapshot()->version());
    EXPECT_NE("", dataset.snapshot()->metadata().id());

    EXPECT_EQ(1, dataset.version_control()->all_branches().size());
    EXPECT_EQ("main", dataset.version_control()->all_branches()[0].name());
}

TEST_F(DatasetTest, create_dataset) {
    auto dataset = deeplake::create_dataset(test_dir);
    EXPECT_EQ(test_dir, dataset.path());

    EXPECT_THROW(deeplake::create_dataset(test_dir), std::runtime_error);
    EXPECT_EQ(0, dataset.snapshot()->version());
    EXPECT_TRUE(std::filesystem::exists(test_dir + "/_deeplake_log"));
    EXPECT_TRUE(std::filesystem::exists(test_dir + "/_deeplake_log/00000000000000000000.json"));
    EXPECT_FALSE(std::filesystem::exists(test_dir + "/_deeplake_log/00000000000000000001.json"));

    std::ifstream ifs(test_dir + "/_deeplake_log/00000000000000000000.json");
    json jf = json::parse(ifs);
    EXPECT_EQ(3, jf.size());
    EXPECT_TRUE(jf[0].contains("protocol"));
    EXPECT_TRUE(jf[1].contains("metadata"));
    EXPECT_TRUE(jf[2].contains("createBranch"));

    EXPECT_EQ(4, dataset.snapshot()->protocol().min_reader_version());
    EXPECT_EQ(4, dataset.snapshot()->protocol().min_writer_version());

    EXPECT_NE("", dataset.snapshot()->metadata().id());
    EXPECT_NE(0, dataset.snapshot()->metadata().created_time());
    EXPECT_FALSE(dataset.snapshot()->metadata().name().has_value());
    EXPECT_FALSE(dataset.snapshot()->metadata().description().has_value());

    EXPECT_EQ(1, dataset.version_control()->all_branches().size());
    EXPECT_EQ("main", dataset.version_control()->all_branches()[0].name());
    EXPECT_EQ("", dataset.version_control()->all_branches()[0].id());
}

TEST_F(DatasetTest, create_branch) {
    auto dataset = deeplake::create_dataset(test_dir);

    EXPECT_EQ(1, dataset.version_control()->all_branches().size());

    auto branch2_snapshot = dataset.create_branch("branch2_snapshot");
    EXPECT_EQ(1, dataset.snapshot()->version());
    EXPECT_EQ(2, dataset.version_control()->all_branches().size());

    EXPECT_EQ("branch2_snapshot", branch2_snapshot->branch().name());
    EXPECT_EQ(1, branch2_snapshot->branch().from_version());
    EXPECT_EQ(deeplake::main_branch().id(), branch2_snapshot->branch().from_id());
}

