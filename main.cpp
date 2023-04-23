#include "Compressor.h"
#include "Decompressor.h"
#include <iostream>
#include <cstring>

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cout << "Got no arguements\n";
        return 1;
    }
    if (std::strcmp(argv[1], "-c") == 0) {
        if (argc < 4) {
            std::cout << "Need more arguements\n";
            return 1;
        }
        std::vector<std::string_view> file_paths;
        for (size_t i = 3; i < argc; ++i) {
            file_paths.push_back(argv[i]);
        }
        try {
            Compressor compressed_file;
            compressed_file.DoCompression(file_paths, argv[2]);
        } catch (const std::exception& exception) {
            std::cout << exception.what() << std::endl;
            return 1;
        } catch (...) {
            std::cout << "Fatal error : unprocessed exception\n";
            return 2;
        }
    } else if (std::strcmp(argv[1], "-d") == 0) {
        if (argc < 3) {
            std::cout << "Need more arguments\n";
            return 1;
        }
        if (argc > 3) {
            std::cout << "Too many arguments\n";
            return 1;
        }
        try {
            Decompressor decompressed_file;
            decompressed_file.DoDecompression(argv[2]);
        } catch (const std::exception& exception) {
            std::cout << exception.what() << std::endl;
            return 1;
        } catch (...) {
            std::cout << "Fatal error : unprocessed exception\n";
            return 2;
        }
    } else if (std::strcmp(argv[1], "-h") == 0) {
        std::cout << "archiver -c archive_name file1 [file2 ...]"
                     " - Compress fil1, file2, ... and save the result in the archive_name file.\n"
                     "archiver -d archive_name"
                     " - Decompress files from archive archive_name and put in current directory.\n";
    } else {
        std::cout << "Unknown command\n";
        return 1;
    }
    return 0;
}
