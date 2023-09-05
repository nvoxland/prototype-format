#include <gtest/gtest.h>
#include <filesystem>
#include "deeplog.hpp"
#include "actions/protocol_action.hpp"
#include "actions/metadata_action.hpp"
#include "last_checkpoint.hpp"
#include <fstream>
#include <string>
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

TEST_F(DeeplogTest, version) {
    auto log = deeplake::deeplog::create(test_dir);
    EXPECT_EQ(0, log->version());
    EXPECT_EQ(0, log->version(deeplake::MAIN_BRANCH_ID));
}

TEST_F(DeeplogTest, branch_by_id) {
    auto log = deeplake::deeplog::create(test_dir);
    EXPECT_EQ("main", log->branch_by_id(deeplake::MAIN_BRANCH_ID).data->name());
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

TEST_F(DeeplogTest, commit_metadata) {
    auto log = deeplake::deeplog::create(test_dir);

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

TEST_F(DeeplogTest, commit_add_file) {
    auto log = deeplake::deeplog::create(test_dir);

    auto action = deeplake::add_file_action("my/path", 3, 45, true);
    log->commit(deeplake::MAIN_BRANCH_ID, log->version(deeplake::MAIN_BRANCH_ID), {&action});

    EXPECT_EQ((std::set<std::string>{"00000000000000000000.json", "00000000000000000001.json"}), list_log_files());
    std::ifstream ifs(test_dir + "/_deeplake_log/00000000000000000001.json");
    nlohmann::json jf = nlohmann::json::parse(ifs);
    EXPECT_EQ(1, jf.size());
    EXPECT_TRUE(jf[0].contains("add"));

    const auto &files = log->data_files(deeplake::MAIN_BRANCH_ID, std::nullopt).data;

    EXPECT_EQ(1, files.size());
    EXPECT_EQ("my/path", files[0].path());
    EXPECT_EQ(3, files[0].size());
    EXPECT_EQ(45, files[0].modification_time());
}

TEST_F(DeeplogTest, commit_create_branch) {
    auto log = deeplake::deeplog::create(test_dir);

    auto action = deeplake::create_branch_action("123", "branch1", deeplake::MAIN_BRANCH_ID, 0);
    log->commit(deeplake::MAIN_BRANCH_ID, log->version(deeplake::MAIN_BRANCH_ID), {&action});

    EXPECT_EQ((std::set<std::string>{"00000000000000000000.json", "00000000000000000001.json"}), list_log_files());
    std::ifstream ifs(test_dir + "/_deeplake_log/00000000000000000001.json");
    nlohmann::json jf = nlohmann::json::parse(ifs);
    EXPECT_EQ(1, jf.size());
    EXPECT_TRUE(jf[0].contains("createBranch"));

    const auto &branches = log->branches().data;

    EXPECT_EQ(2, branches->size());
    EXPECT_EQ("", (*branches)[0].id());
    EXPECT_EQ("main", (*branches)[0].name());

    EXPECT_EQ("123", (*branches)[1].id());
    EXPECT_EQ("branch1", (*branches)[1].name());
}

TEST_F(DeeplogTest, checkpoint) {
    auto log = deeplake::deeplog::create(test_dir);

    auto original_metadata = log->metadata().data;
    for (int i=0; i<=20; ++i) {
        auto action = deeplake::metadata_action(original_metadata->id(), "name " + std::to_string(i), "desc "+std::to_string(i), original_metadata->created_time());
        log->commit(deeplake::MAIN_BRANCH_ID, log->version(deeplake::MAIN_BRANCH_ID), {&action});
    }

    EXPECT_EQ(21, log->version());
    EXPECT_EQ(22, list_log_files().size());

    EXPECT_EQ(original_metadata->id(), log->metadata().data->id());
    EXPECT_EQ(original_metadata->created_time(), log->metadata().data->created_time());
    EXPECT_EQ("name 20", log->metadata().data->name());
    EXPECT_EQ("desc 20", log->metadata().data->description());

    log->checkpoint();

    EXPECT_TRUE(list_log_files().contains("00000000000000000021.checkpoint.parquet"));
    EXPECT_TRUE(list_log_files().contains("_last_checkpoint.json"));

    std::ifstream ifs(test_dir + "/_deeplake_log/_last_checkpoint.json");
    deeplake::last_checkpoint checkpoint_content = nlohmann::json::parse(ifs).template get<deeplake::last_checkpoint>();
    EXPECT_EQ(21, checkpoint_content.version);


    //delete json files so loads after checkpoint doesn't use it
    for (auto file : list_log_files()) {
        if (file.ends_with(".json")) {
            std::filesystem::remove(test_dir+"/_deeplake_log/"+file);
        }
    }
    EXPECT_FALSE(list_log_files().contains("00000000000000000000.json"));

    auto new_log = deeplake::deeplog::open(test_dir);
    EXPECT_EQ(21, new_log->version());
    EXPECT_EQ(original_metadata->id(), new_log->metadata().data->id());
    EXPECT_EQ("name 20", new_log->metadata().data->name());
}