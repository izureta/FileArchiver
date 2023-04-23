#pragma once
#include "Library/BitReader.h"
#include <string_view>
#include <vector>

class Compressor {
public:
    void DoCompression(std::vector<std::string_view> fine_names, std::string_view archive_name);
};