#include "Compressor.h"
#include "Library/Trie.h"
#include "Library/BitWriter.h"
#include "Library/SpecialCharacters.h"
#include "Library/Heap.h"
#include <iterator>
#include <queue>
#include <map>
#include <tuple>
#include <algorithm>
#include <filesystem>

#include <iostream>

struct CompareByCount {
    bool operator()(const NodePtr a, const NodePtr b) const {
        return a->GetCount() < b->GetCount();
    }
};

std::string GetFilename(std::string_view file_path) {
    return std::filesystem::path(file_path).filename();
}

Trie BuildTrie(std::istream& input, std::string_view file_name) {
    BitReader bit_reader(input);
    std::map<uint16_t, size_t> count_symbols;
    ++count_symbols[SPECIAL_CHARACTERS::FILENAME_END];   //archive control special characters
    ++count_symbols[SPECIAL_CHARACTERS::ONE_MORE_FILE];
    count_symbols[SPECIAL_CHARACTERS::ARCHIVE_END] = 1;
    for (const unsigned char c : file_name) {
        ++count_symbols[c];
    }
    while (bit_reader.CanRead()) {
        uint16_t block = bit_reader.ReadNBits(8);
        ++count_symbols[block];
    }
    Heap<NodePtr, CompareByCount> node_priority_queue;
    for (const auto [symbol, count] : count_symbols) {
        node_priority_queue.push(static_cast<NodePtr>(new Trie::Node(symbol, count, true)));
    }
    while (node_priority_queue.size() > 1) {
        NodePtr a = node_priority_queue.top();
        node_priority_queue.pop();
        NodePtr b = node_priority_queue.top();
        node_priority_queue.pop();
        node_priority_queue.push(static_cast<NodePtr>(new Trie::Node(a, b)));
    }
    NodePtr root = node_priority_queue.top();
    node_priority_queue.pop();
    return Trie(root);
}

using CodeTable = std::map<uint16_t, std::vector<bool>>;

void DFS(NodePtr current_node, std::vector<bool> current_code, CodeTable& code_table) {
    if (current_node->IsTerminal()) {
        if (current_code.empty()) {
            current_code.push_back(0);
        }
        code_table[current_node->GetSymbol()] = current_code;
        return;
    }
    for (size_t i = 0; i < 2; ++i) {
        current_code.push_back(i);
        DFS(current_node->Child(i), current_code, code_table);
        current_code.pop_back();
    }
}

CodeTable MakeCodeTable(Trie trie) {
    CodeTable code_table;
    std::vector<bool> code;
    DFS(trie.GetRoot(), code, code_table);
    return code_table;
}

struct Code {
    std::vector<bool> code;
    uint16_t symbol;
};

using ListOfCodes = std::vector<Code>;

ListOfCodes BuildCanonicalCode(CodeTable& code_table) {
    std::vector<Code> codes;
    for (const auto [symbol, code] : code_table) {
        codes.push_back({code, symbol});
    }
    auto Compare = [](const Code a, const Code b) {
        return std::tuple(a.code.size(), a.symbol) < std::tuple(b.code.size(), b.symbol);
    };
    sort(codes.begin(), codes.end(), Compare);
    uint16_t current_code_value = 0;
    std::vector<bool> current_code;
    code_table.clear();
    ListOfCodes canonical_codes;
    size_t prev_code_size = 1;
    for (const auto [code, symbol] : codes) {
        current_code.clear();
        if (current_code_value != 0) {
            for (size_t i = prev_code_size; i < code.size(); ++i) {
                current_code_value *= 2;
            }
        }
        for (size_t i = 0; i < code.size(); ++i) {
            current_code.push_back(current_code_value >> (code.size() - i - 1) & 1);
        }
        code_table[symbol] = current_code;
        canonical_codes.push_back({current_code, symbol});
        prev_code_size = canonical_codes.back().code.size();
        ++current_code_value;
    }
    for (auto& [code, symbol] : canonical_codes) {
        std::reverse(code.begin(), code.end());
    }
    return canonical_codes;
}

std::pair<CodeTable, ListOfCodes> MakeCanonicalCodeTable(std::istream& input, std::string_view file_name) {
    Trie trie = BuildTrie(input, file_name);
    CodeTable code_table = MakeCodeTable(trie);
    ListOfCodes canonical_codes = BuildCanonicalCode(code_table);
    return {code_table, canonical_codes};
}

void Compress(std::istream& input, BitWriter& bit_writer, std::string_view file_name, const CodeTable& code_table,
              ListOfCodes canonical_codes, bool is_last_file) {
    bit_writer.WriteNBits(9, code_table.size());
    for (const auto [code, symbol] : canonical_codes) {
        bit_writer.WriteNBits(9, symbol);
    }
    size_t current_size = 0;
    size_t current_size_count = 0;
    for (const auto [code, symbol] : canonical_codes) {
        if (code.size() > current_size) {
            if (current_size_count != 0) {
                bit_writer.WriteNBits(9, current_size_count);
            }
            for (size_t i = current_size + 1; i < code.size(); ++i) {
                bit_writer.WriteNBits(9, 0);
            }
            current_size = code.size();
            current_size_count = 1;
        } else {
            ++current_size_count;
        }
    }
    if (current_size_count > 0) {
        bit_writer.WriteNBits(9, current_size_count);
    }

    for (const unsigned char c : file_name) {
        bit_writer.WriteNBits(code_table.at(c));
    }

    bit_writer.WriteNBits(code_table.at(SPECIAL_CHARACTERS::FILENAME_END));

    BitReader bit_reader(input);
    while (bit_reader.CanRead()) {
        uint16_t block = bit_reader.ReadNBits(8);
        bit_writer.WriteNBits(code_table.at(block));
    }
    if (!is_last_file) {
        bit_writer.WriteNBits(code_table.at(SPECIAL_CHARACTERS::ONE_MORE_FILE));
    } else {
        bit_writer.WriteNBits(code_table.at(SPECIAL_CHARACTERS::ARCHIVE_END));
    }
}

void Compressor::DoCompression(std::vector<std::string_view> file_paths, std::string_view archive_name) {
    std::ofstream archive;
    archive.open(archive_name.data(), std::ios_base::binary);
    if (!archive.is_open()) {
        throw std::ios_base::failure("Couldn't open archive\n");
    }
    BitWriter bit_writer(archive);
    for (size_t i = 0; i < file_paths.size(); ++i) {
        std::string file_name = GetFilename(file_paths[i]);
        std::string_view file_path = file_paths[i];
        std::ifstream file;
        file.open(file_path.data(), std::ios_base::binary);
        if (!file.is_open()) {
            throw std::ios_base::failure("Couldn't open a file\n");
        }
        auto[code_table, canonical_codes] = MakeCanonicalCodeTable(file, file_name);

        file.close();
        file.open(file_path.data(), std::ios_base::binary);
        if (!file.is_open()) {
            throw std::ios_base::failure("Couldn't open file second time\n");
        }
        Compress(file, bit_writer, file_name, code_table, canonical_codes, i + 1 == file_paths.size());
        std::cout << "Compressed " << file_name << std::endl;
    }
}