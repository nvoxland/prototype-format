#include <gtest/gtest.h>
#include <filesystem>
#include "deeplog.hpp"
#include "actions/protocol_action.hpp"
#include "actions/metadata_action.hpp"
#include <fstream>
#include <nlohmann/json.hpp>

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

    std::set<std::string> list_log_files() {
        auto files = std::set<std::string>();
        std::filesystem::path dir_path = {test_dir + "/_deeplake_log/"};
        for (const auto &entry: std::filesystem::directory_iterator(dir_path)) {
            files.insert(entry.path().string().substr((test_dir + "/_deeplake_log/").size()));
        }

        return files;
    }

    std::string test_dir = "tmp/test";
};


TEST_F(DeeplogTest, create) {
    auto log = deeplake::deeplog::create(test_dir);

    EXPECT_EQ(test_dir, log->path());
    EXPECT_TRUE(std::filesystem::exists({test_dir + "/_deeplake_log/"}));
    EXPECT_EQ(std::set<std::string>{"00000000000000000000.json"}, list_log_files());

    std::ifstream ifs(test_dir + "/_deeplake_log/00000000000000000000.json");
    nlohmann::json jf = nlohmann::json::parse(ifs);
    EXPECT_EQ(3, jf.size());
    EXPECT_TRUE(jf[0].contains("protocol"));
    EXPECT_TRUE(jf[1].contains("metadata"));
    EXPECT_TRUE(jf[2].contains("createBranch"));


    EXPECT_EQ(1, log->branches().data->size());
    EXPECT_EQ("main", log->branches().data->begin()->name());
    EXPECT_EQ(4, log->protocol().data->min_reader_version());
    EXPECT_EQ(4, log->protocol().data->min_writer_version());

    EXPECT_NE("", log->metadata().data->id());
    EXPECT_NE(0, log->metadata().data->created_time());
    EXPECT_FALSE(log->metadata().data->name().has_value());
    EXPECT_FALSE(log->metadata().data->description().has_value());

    const auto files = log->data_files(deeplake::MAIN_BRANCH_ID, std::nullopt);
    EXPECT_EQ(0, files.version);
    EXPECT_EQ(0, files.data.size());

    EXPECT_THROW(deeplake::deeplog::create(test_dir), std::runtime_error) << "Should not be able to create log twice";
}


TEST_F(DeeplogTest, open) {
    auto log = deeplake::deeplog::open(test_dir);

    EXPECT_EQ(test_dir, log->path());
}

TEST_F(DeeplogTest, commit_protocol) {
    auto log = deeplake::deeplog::create(test_dir);

    auto action = deeplake::protocol_action(5, 6);
    log->commit(deeplake::MAIN_BRANCH_ID, log->version(deeplake::MAIN_BRANCH_ID), {&action});

    EXPECT_EQ((std::set<std::string>{"00000000000000000000.json", "00000000000000000001.json"}), list_log_files());
    std::ifstream ifs(test_dir + "/_deeplake_log/00000000000000000001.json");
    nlohmann::json jf = nlohmann::json::parse(ifs);
    EXPECT_EQ(1, jf.size());
    EXPECT_TRUE(jf[0].contains("protocol"));

    EXPECT_EQ(5, log->protocol().data->min_reader_version());
    EXPECT_EQ(6, log->protocol().data->min_writer_version());
}

TEST_F(DeeplogTest, comit_metadata) {
    auto log = deeplake::deeplog::create(test_dir);

    auto branch = deeplake::branches::main();

    auto original_metadata = log->metadata().data;
    auto action = deeplake::metadata_action(original_metadata->id(), "new name", "new desc", original_metadata->created_time());
    log->commit(deeplake::MAIN_BRANCH_ID, log->version(deeplake::MAIN_BRANCH_ID), {&action});

    EXPECT_EQ((std::set<std::string>{"00000000000000000000.json", "00000000000000000001.json"}), list_log_files());
    std::ifstream ifs(test_dir + "/_deeplake_log/00000000000000000001.json");
    nlohmann::json jf = nlohmann::json::parse(ifs);
    EXPECT_EQ(1, jf.size());
    EXPECT_TRUE(jf[0].contains("metadata"));

    EXPECT_EQ(original_metadata->id(), log->metadata().data->id());
    EXPECT_EQ(original_metadata->created_time(), log->metadata().data->created_time());
    EXPECT_EQ("new name", log->metadata().data->name());
    EXPECT_EQ("new desc", log->metadata().data->description());
}
