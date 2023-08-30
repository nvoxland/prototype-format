#include <gtest/gtest.h>
#include <filesystem>
#include "deeplog.hpp"

class DeeplogTest : public ::testing::Test {
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

TEST_F(DeeplogTest, open) {
    const std::shared_ptr<deeplake::deeplog> &log = deeplake::deeplog::open(test_dir);

    EXPECT_EQ(test_dir, log->path());
}