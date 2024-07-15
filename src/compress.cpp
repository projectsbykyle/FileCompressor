#include "huffman_helper.h"

//g++ -Wall -g -O0 -std=c++11 compress.cpp huffman_helper.cpp Helper.cpp -o compress_test
int main(int argc, char* argv[]) {
    // your program's main execution code
    const std::vector<int> *freq = count_freq(argv[1]);
    HCTree tree; 
    tree.build(*freq); 
    output(tree, freq, argv[1], argv[2]); 
    delete freq; 
}
