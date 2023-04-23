#pragma once
#include <fstream>

class BitReader {
public:
    BitReader(std::istream& file) : input_(file) {}
    bool CanRead();
    uint16_t ReadOneBit();
    uint16_t ReadNBits(size_t n); //not more than 16 bits

private:
    std::istream& input_;
    uint16_t buffer_ = 0;
    size_t buffer_size_ = 0;
};
