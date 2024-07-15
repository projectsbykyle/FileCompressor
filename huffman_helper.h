#include <stdlib.h> 
#include "HCTree.hpp"
#include "Helper.hpp"

//Open file_name and count the frequencies of each character (one byte). Return a std::vector where index i is the occurences of byte i
std::vector<int>* count_freq(const char *file_name); 

//Compares two nodes and returns the one with the smaller count
struct compare_node{
    bool operator()(const HCNode *A, const HCNode *B){
            return A->count > B->count; 
    }
}; 

//Compress tree
void compress_tree(HCTree &tree, FancyOutputStream& out); 

//Given an HCNode, delete it and all of its subtrees fro memory
void clear(HCNode *node); 

//Compress file to output_file 
void output(HCTree& tree, const std::vector<int> *freq, const char *file_name, const char *output_file); 

//Uncompress compressed file 
void uncompress(const char *compressed_file, const char *output_file);

