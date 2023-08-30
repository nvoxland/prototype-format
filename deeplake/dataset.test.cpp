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
    auto dataset = deeplake::dataset::create(test_dir);

    EXPECT_EQ(test_dir, dataset->path());
    EXPECT_EQ(0, dataset->snapshot()->version());
    EXPECT_NE("", dataset->metadata()->id());

    EXPECT_EQ(1, dataset->branches()->size());
    EXPECT_EQ("main", dataset->branches()->begin()->name());
}

TEST_F(DatasetTest, create) {
    auto dataset = deeplake::dataset::create(test_dir);
    EXPECT_EQ(test_dir, dataset->path());

    EXPECT_TRUE(std::filesystem::exists(test_dir + "/_deeplake_log"));
    EXPECT_TRUE(std::filesystem::exists(test_dir + "/_deeplake_log/00000000000000000000.json")) << "Should create initial commit file";
    EXPECT_FALSE(std::filesystem::exists(test_dir + "/_deeplake_log/00000000000000000001.json"));

    std::ifstream ifs(test_dir + "/_deeplake_log/00000000000000000000.json");
    json jf = json::parse(ifs);
    EXPECT_EQ(3, jf.size());
    EXPECT_TRUE(jf[0].contains("protocol"));
    EXPECT_TRUE(jf[1].contains("metadata"));
    EXPECT_TRUE(jf[2].contains("createBranch"));

    EXPECT_THROW(deeplake::dataset::create(test_dir), std::runtime_error) << "Should not be able to create dataset twice";

    EXPECT_EQ(0, dataset->snapshot()->version());


    EXPECT_EQ(4, dataset->protocol()->min_reader_version());
    EXPECT_EQ(4, dataset->protocol()->min_writer_version());

    EXPECT_NE("", dataset->metadata()->id());
    EXPECT_NE(0, dataset->metadata()->created_time());
    EXPECT_FALSE(dataset->metadata()->name().has_value());
    EXPECT_FALSE(dataset->metadata()->description().has_value());

    EXPECT_EQ(1, dataset->branches()->size());
    EXPECT_EQ("main", dataset->branches()->begin()->name());
    EXPECT_EQ("", dataset->branches()->begin()->id());
}



//TEST(E2EDatasetTest, e2e) {
////    auto dataset = deeplake::create_dataset("../test-ds");
//
//    auto dataset = deeplake::dataset("../test-ds");
//
////        dataset->create_branch("alt1");
//    dataset->checkout_branch("alt1");
////        dataset->create_branch("alt2");
////    dataset->checkout_branch("alt2");
//
////    dataset->add_data(std::vector<std::string>{"a", "b", "c"});
////    dataset->add_data(std::vector<std::string>{"d"});
////    dataset->add_data(std::vector<std::string>{"e"});
////    dataset->add_data(std::vector<std::string>{"f"});
////    dataset->add_data(std::vector<std::string>{"g"});
//
//    auto snapshot = dataset->snapshot();
//    std::cout << "Version: " << snapshot->version() << std::endl;
//    std::cout << "Data:" << std::endl;
//    for (const auto &data: snapshot->scan()) {
//        std::cout << "   " << data << std::endl;
//    }
//
//
//}
