#include "Decompressor.h"
#include "Library/BitReader.h"
#include "Library/BitWriter.h"
#include "Library/SpecialCharacters.h"
#include "Library/Trie.h"
#include <vector>
#include <map>
#include <algorithm>

#include <iostream>

using BitCode = std::vector<bool>;
using ReverseCodeTable = std::map<BitCode, uint16_t>;

ReverseCodeTable MakeReverseCodeTable(BitReader &bit_reader) {
    size_t symbols_count = bit_reader.ReadNBits(9);
    std::vector<uint16_t> symbols;
    for (size_t i = 0; i < symbols_count; ++i) {
        symbols.push_back(bit_reader.ReadNBits(9));
    }
    size_t remaining_symbols_count = symbols_count;
    size_t prev_code_size = 1;
    uint16_t current_code_value = 0;
    ReverseCodeTable code_table;
    size_t last_symbol_index = 0;
    for (size_t i = 0;; ++i) {
        if (remaining_symbols_count == 0) {
            break;
        }
        size_t current_size_symbols_count = bit_reader.ReadNBits(9);
        remaining_symbols_count -= current_size_symbols_count;
        while (current_size_symbols_count != 0) {
            if (current_code_value != 0) {
                for (size_t j = prev_code_size; j < i + 1; ++j) {
                    current_code_value *= 2;
                }
            }
            BitCode current_code;
            for (size_t j = 0; j <= i; ++j) {
                current_code.push_back(current_code_value >> (i - j) & 1);
            }
            code_table[current_code] = symbols[last_symbol_index++];
            prev_code_size = i + 1;
            ++current_code_value;
            --current_size_symbols_count;
        }
    }
    return code_table;
}

Trie BuildTrie(ReverseCodeTable &code_table) {
    NodePtr root(new Trie::Node(0, 0, false));
    Trie trie(root);
    for (const auto[code, symbol]: code_table) {
        trie.AddCode(code, symbol);
    }
    return trie;
}

std::string GetFileName(BitReader &bit_reader, Trie &trie, const ReverseCodeTable &code_table) {
    NodePtr current_node = trie.GetRoot();
    BitCode current_code;
    std::string file_name;
    while (true) {
        uint16_t bit = bit_reader.ReadOneBit();
        current_node = current_node->Child(bit);
        current_code.push_back(bit);
        if (current_node->IsTerminal()) {
            uint16_t symbol = code_table.at(current_code);
            if (symbol == SPECIAL_CHARACTERS::FILENAME_END) {
                break;
            }
            file_name += symbol;
            current_code.clear();
            current_node = trie.GetRoot();
        }
    }
    return file_name;
}

bool Decompress(BitReader &bit_reader, std::ostream &output, Trie trie, ReverseCodeTable code_table) {
    BitWriter bit_writer(output);
    NodePtr current_node = trie.GetRoot();
    BitCode current_code;
    while (true) {
        uint16_t bit = bit_reader.ReadOneBit();
        current_node = current_node->Child(bit);
        current_code.push_back(bit);
        if (current_node->IsTerminal()) {
            uint16_t symbol = code_table.at(current_code);
            if (symbol == SPECIAL_CHARACTERS::ONE_MORE_FILE) {
                return false;
            }
            if (symbol == SPECIAL_CHARACTERS::ARCHIVE_END) {
                return true;
            }
            if (code_table[current_code] != 0) {
                bit_writer.WriteNBits(8, code_table[current_code]);
            }
            current_code.clear();
            current_node = trie.GetRoot();
        }
    }
}

void Decompressor::DoDecompression(std::string_view archive_name) {
    std::ifstream archive;
    archive.open(archive_name.data(), std::ios_base::binary);
    BitReader bit_reader(archive);
    if (!archive.is_open()) {
        throw std::ios_base::failure("Couldn't open archive\n");
    }
    while (true) {
        ReverseCodeTable code_table = MakeReverseCodeTable(bit_reader);
        Trie trie = BuildTrie(code_table);
        std::string file_name = GetFileName(bit_reader, trie, code_table);
        std::ofstream output(file_name);
        if (Decompress(bit_reader, output, trie, code_table)) {
            std::cout << "Decompressed " << file_name << std::endl;
            return;
        }
        std::cout << "Decompressed " << file_name << std::endl;
    }
}
