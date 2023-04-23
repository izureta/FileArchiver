#include "BitWriter.h"

BitWriter::~BitWriter() {
    if (buffer_size_ > 0) {
        while (buffer_size_ % 8 != 0) {
            ++buffer_size_;
        }
    }
    while (buffer_size_ != 0) {
        uint8_t byte = 255 & buffer_;
        output_ << byte;
        buffer_ >>= 8;
        buffer_size_ -= 8;
    }
}

void BitWriter::WriteOneBit(size_t bit) {
    buffer_ += bit * (1 << buffer_size_);
    ++buffer_size_;
    while (buffer_size_ >= 8) {
        uint8_t byte = 255 & buffer_;
        output_ << byte;
        buffer_ >>= 8;
        buffer_size_ -= 8;
    }
}

void BitWriter::WriteNBits(size_t n, uint16_t bits) {
    if (n > 16) {
        throw std::invalid_argument("Can't write more than 16 bits at a time");
    }
    for (size_t i = 0; i < n; ++i) {
        WriteOneBit(bits >> i & 1);
    }
}

void BitWriter::WriteNBits(std::vector<bool> bits) {
    if (bits.size() > 16) {
        throw std::invalid_argument("Can't write more than 16 bits at a time");
    }
    for (const bool bit : bits) {
        WriteOneBit(bit);
    }
}
