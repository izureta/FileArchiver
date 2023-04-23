cmake_minimum_required(VERSION 3.16)
project(archiver)

set(CMAKE_CXX_STANDARD 20)

add_subdirectory(library)
add_subdirectory(tests)

add_executable(archiver
        main.cpp
        Compressor.cpp Decompressor.cpp Decompressor.h)

target_link_libraries(archiver HuffmanArchiverLibrary)