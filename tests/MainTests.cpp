#include "../Compressor.cpp"
#include "../Decompressor.cpp"
#define CATCH_CONFIG_MAIN
#include "catch.hpp"

TEST_CASE("HuffmanTest") {
    std::vector<std::string> ar_data = {"123", "1[29vn29rvn29-3vnc`poeun2er9ucn02er", "---  -- - - - "};
    for (const auto& data : ar_data) {
        std::string_view input = "..\\..\\tests\\TestInput";
        std::string_view output = "..\\..\\tests\\TestOutput";
        std::ofstream input_file(input.data());
        REQUIRE(input_file.is_open());
        input_file << data;
        input_file.close();
        Compressor compressor;
        compressor.DoCompression({input}, output);
        Decompressor decompressor;
        decompressor.DoDecompression(output);
        std::ifstream result_file(input.data());
        REQUIRE(result_file.is_open());
        std::string check;
        getline(result_file, check);
        REQUIRE(check == data);
    }
}

TEST_CASE("MultipleFilesHuffmanTest") {
    std::vector<std::pair<std::string, std::string>> ar_data = {{"123", "qwerty"},
                                                                {"1[29vn29rvn29-3vnc`poeun2er9ucn02er", "-14 -1491"},
                                                                {"=1-2= 410efef", "---  -- - - - "}};
    for (const auto& data : ar_data) {
        std::string_view input1 = "..\\..\\tests\\TestInput1";
        std::string_view input2 = "..\\..\\tests\\TestInput2";
        std::string_view output = "..\\..\\tests\\TestOutput";
        std::ofstream input_file1(input1.data());
        std::ofstream input_file2(input2.data());
        REQUIRE(input_file1.is_open());
        REQUIRE(input_file2.is_open());
        input_file1 << data.first;
        input_file1.close();
        input_file2 << data.second;
        input_file2.close();
        Compressor compressor;
        compressor.DoCompression({input1, input2}, output);
        Decompressor decompressor;
        decompressor.DoDecompression(output);
        std::ifstream result_file1(input1.data());
        std::ifstream result_file2(input2.data());
        std::string check;
        getline(result_file1, check);
        REQUIRE(check == data.first);
        getline(result_file2, check);
        REQUIRE(check == data.second);
    }
}

TEST_CASE("MakeCanonicalCodeTableTests") {
    std::stringstream s1, s2, s3;
    s1 << "123";
    auto m = MakeCanonicalCodeTable(s1, "");
    s2 << "1123";
    m = MakeCanonicalCodeTable(s2, "");
    REQUIRE(m.first == CodeTable{{0, {0, 1, 0}}, {'1', {0, 0}}, {'2', {0, 1, 1}}, {'3', {1, 0, 0}}, {256, {1, 0, 1}},
                                 {257, {1, 1, 0}}, {258, {1, 1, 1}}});
    s3 << "1231";
    m = MakeCanonicalCodeTable(s3, "");
    REQUIRE(m.first == CodeTable{{0, {0, 1, 0}}, {'1', {0, 0}}, {'2', {0, 1, 1}}, {'3', {1, 0, 0}}, {256, {1, 0, 1}},
                                 {257, {1, 1, 0}}, {258, {1, 1, 1}}});
}