# FileArchiver
File archiver based on Huffman lossless compression algorithm

How to build and run:
```
mkdir build
cd build
cmake ..
make
./archiver -h
```
To test:
```
cd tests
make
./tests
```
Compressing:
```
./archiver -c ARCHIVE_NAME FILE_NAME1 FILE_NAME2 ...
```
Creates or overwrites existing archive file and stores given compressed files.

Decompressing:
```
./archiver -d ARCHIVE_NAME
```
Decompresses files stored in archive and puts them in current directory (or overwrites existing files with same names).
