#include <gtest/gtest.h>
#include "scan_result.hpp"
#include <fstream>

class ScanResultTest : public ::testing::Test {
protected:
    void SetUp() override {
        if (std::filesystem::exists(test_dir)) {
            std::filesystem::remove_all(test_dir);
        }
        std::filesystem::create_directories(test_dir);
    }

    void TearDown() override {
        if (std::filesystem::exists(test_dir)) {
            std::filesystem::remove_all(test_dir);
        }
    }

    std::string test_dir = "tmp/test";
};

TEST_F(ScanResultTest, range_support) {
    auto a_stream = std::ofstream(test_dir + "/a.txt");
    a_stream << "Value 1\nValue 2\nValue 3";
    a_stream.close();

    auto b_stream = std::ofstream(test_dir + "/b.txt");
    b_stream << "Value 4\nValue 5\nValue 6";
    b_stream.close();

    auto c_stream = std::ofstream(test_dir + "/c.txt");
    c_stream << "Value 7\nValue 8\nValue 9";
    c_stream.close();

    auto result = deeplake::scan_result(test_dir, std::vector<deeplake::add_file_action>{
            deeplake::add_file_action("a.txt", 3, 5, true),
            deeplake::add_file_action("b.txt", 3, 5, true),
            deeplake::add_file_action("c.txt", 3, 5, true),
    });

    EXPECT_EQ("Value 1", result[0]);
    EXPECT_EQ("Value 2", result[1]);
    EXPECT_EQ("Value 9", result[8]);

    auto out_data = std::vector<std::string>();
    for (auto out: result) {
        out_data.emplace_back(out);
    }

    EXPECT_EQ("Value 1", out_data[0]);
    EXPECT_EQ("Value 2", out_data[1]);
    EXPECT_EQ("Value 9", out_data[8]);


}