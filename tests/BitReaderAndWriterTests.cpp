#include "BitReader.h"
#include "BitWriter.h"
#include <vector>
#include <sstream>
#include "catch.hpp"

TEST_CASE("BitReaderTest") {
    std::vector<char> data = {0x12, 0x25};
    std::stringstream input;
    input << data[0] << data[1];
    BitReader bit_reader1(input);
    REQUIRE(bit_reader1.ReadNBits(16) == (0x12 + (0x25 << 8)));
    input << data[0] << data[1];
    BitReader bit_reader2(input);
    try {
        bit_reader2.ReadNBits(17);
    } catch (std::invalid_argument) {
        REQUIRE(true);
    }
    input << data[0] << data[1];
    BitReader bit_reader3(input);
    try {
        bit_reader3.ReadNBits(9);
        bit_reader3.ReadNBits(8);
    } catch (std::out_of_range) {
        REQUIRE(true);
    }
}

TEST_CASE("TestReadAndWrite") {
    std::stringstream input, output;
    {
        BitWriter bit_writer(input);
        bit_writer.WriteNBits(9, 14);
    }
    BitReader bit_reader(input);
    REQUIRE(bit_reader.ReadNBits(9) == 14);
}

TEST_CASE("TestReverseReadAndWrite") {
    std::stringstream input, output;
    {
        BitWriter bit_writer(input);
        bit_writer.WriteNBits(9, 14);
    }
    BitReader bit_reader(input);
    REQUIRE(bit_reader.ReadNBits(9) == 14);
}