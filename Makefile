# use g++ with C++11 support
CXX=g++
CXXFLAGS?=-Wall -pedantic -g -O0 -std=c++11
OUTFILES=compress uncompress

all: $(OUTFILES)

compress: compress.cpp Helper.cpp Helper.hpp HCTree.hpp huffman_helper.cpp
	$(CXX) $(CXXFLAGS) -o compress compress.cpp Helper.cpp huffman_helper.cpp

uncompress: uncompress.cpp Helper.cpp Helper.hpp HCTree.hpp huffman_helper.cpp
	$(CXX) $(CXXFLAGS) -o uncompress uncompress.cpp Helper.cpp huffman_helper.cpp

gprof: compress.cpp uncompress.cpp Helper.cpp Helper.hpp HCTree.hpp huffman_helper.cpp
	make clean
	$(CXX) $(CXXFLAGS) -pg -o compress compress.cpp Helper.cpp huffman_helper.cpp
	$(CXX) $(CXXFLAGS) -pg -o uncompress uncompress.cpp Helper.cpp huffman_helper.cpp

clean:
	rm -f $(OUTFILES) *.o

