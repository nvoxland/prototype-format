#include <gtest/gtest.h>
#include <filesystem>
#include "dataset.hpp"

using namespace std;

class DatasetTest : public ::testing::Test {
protected:
    void TearDown() override {
        if (filesystem::exists(test_dir)) {
            filesystem::remove_all(test_dir);
        }
    }

    filesystem::path test_dir = "tmp/test";
};

TEST_F(DatasetTest, open) {
//    EXPECT_THROW(deeplake::dataset(test_dir), std::runtime_error);

    deeplake::create(test_dir);

    auto dataset = deeplake::dataset(test_dir);
    EXPECT_EQ(test_dir, dataset.path());
}

TEST_F(DatasetTest, create) {
    auto dataset = deeplake::create(test_dir);
    EXPECT_EQ(test_dir, dataset.path());

    EXPECT_THROW(deeplake::create(test_dir), std::runtime_error);
    EXPECT_TRUE(filesystem::exists(test_dir.append("_deeplake_log")));
    EXPECT_TRUE(filesystem::exists(test_dir.append("_deeplake_log").append("00000000000000000000.json")));
    EXPECT_FALSE(filesystem::exists(test_dir.append("_deeplake_log").append("00000000000000000001.json")));
}

