#include "BitReader.h"

bool BitReader::CanRead() {
    return !input_.eof() || buffer_size_ > 0;
}

uint16_t BitReader::ReadOneBit() {
    if (buffer_size_ == 0) {
        char byte = 0;
        if (CanRead()) {
            input_.read(&byte, 1);
        } else {
            return 0;
        }
        buffer_ = static_cast<uint8_t>(byte);
        buffer_size_ += 8;
    }
    uint8_t bit = buffer_ & 1;
    buffer_ >>= 1;
    --buffer_size_;
    return bit;
}

uint16_t BitReader::ReadNBits(size_t n)  { //not more than 16 bits
    if (n > 16) {
        throw std::invalid_argument("Can't read more than 16 bits at a time");
    }
    uint16_t block = 0;
    for (size_t i = 0; i < n; ++i) {
        if (CanRead()) {
            block += ReadOneBit() * (1 << i);
        } else {
            break;
        }
    }
    return block;
}

