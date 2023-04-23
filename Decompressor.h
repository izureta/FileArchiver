#pragma once
#include <fstream>
#include <string_view>

class Decompressor {
public:
    void DoDecompression(std::string_view archive_name);
};

