set(SRC_FILES BitReader.cpp Trie.cpp BitWriter.cpp SpecialCharacters.h Heap.h)

add_library(HuffmanArchiverLibrary ${SRC_FILES})
target_include_directories(HuffmanArchiverLibrary PUBLIC ./)