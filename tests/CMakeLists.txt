find_library(HuffmanArchiverLibrary ../library)

add_executable(tests MainTests.cpp BitReaderAndWriterTests.cpp)
target_link_libraries(tests HuffmanArchiverLibrary)