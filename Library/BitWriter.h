#pragma once
#include <fstream>
#include <vector>

class BitWriter {
public:
    BitWriter(std::ostream& file) : output_(file) {}
    ~BitWriter();
    void WriteOneBit(size_t bit);
    void WriteNBits(size_t n, uint16_t bits); //not more than 16 bits
    void WriteNBits(std::vector<bool>);

private:
    std::ostream& output_;
    uint16_t buffer_ = 0;
    size_t buffer_size_ = 0;
};
