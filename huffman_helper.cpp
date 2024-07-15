#include "huffman_helper.h"
#include "Helper.hpp"
#include <queue>
#include <stdlib.h>
#include <string>
#include <algorithm>
#include <stack> 

/**
* Use the Huffman algorithm to build a Huffman coding tree.
* PRECONDITION: freqs is a vector of ints, such that freqs[i] is the frequency of occurrence of byte i in the input file.
* POSTCONDITION: root points to the root of the trie, and leaves[i] points to the leaf node containing byte i.
*/

std::vector<int>* count_freq(const char *file_name){
    FancyInputStream stream(file_name); 
    std::vector<int> *freq = new std::vector<int>(256, 0); 
    //read_byte() returns -1 when there are no more bytes to read
    for(int cb = stream.read_byte(); cb != -1; cb = stream.read_byte()){
        //printf("%d\n", cb); 
        (*freq)[cb] = (*freq)[cb]+1; 
    }
    stream.reset(); 
    return freq; 
}

void HCTree::build(const vector<int>& freqs){
    //printf("C\n");
    std::priority_queue<HCNode*, std::vector<HCNode*>, compare_node> pq; 
    //printf("A\n");
    for(int i = 0; i < freqs.size(); i++){
        if(freqs[i]){
            HCNode *node = new HCNode(freqs[i], i); 
            //HEREEEEEEE
            size++; 
            leaves[i] = node; 
            node->c0 = nullptr; 
            node->c1 = nullptr; 
            pq.push(node); 
        }
        else
            leaves[i] = nullptr; 
    }
        //printf("B\n");

    while(pq.size() > 1){
        HCNode *A = pq.top();
        pq.pop(); 
        HCNode *B = pq.top();
        pq.pop(); 
        HCNode *C = new HCNode(A->count + B->count, 0); 
        //HEREEEEEE
        size++; 
        A->p = C;
        B->p = C; 
        C->c0 = A; 
        C->c1 = B; 
        pq.push(C); 
    }
    if(!pq.empty()){
        root = pq.top();
        pq.pop();
    }
    for(int i = 0; i < leaves.size(); i++){
        HCNode *node = leaves[i]; 
        if(node != nullptr)
            printf("%c: %d\n", (char) i, node->count); 
    }
    
    //Print paths of each node
    
    for(int i = 0; i < leaves.size(); i++){
        HCNode *node = leaves[i]; 
        if(node != nullptr){
            std::string path = ""; 
            while(node != root){
                HCNode *p = node->p; 
                std::string b = (p->c0 == node) ? "0" : "1"; 
                path += b; 
                node = p; 
            }
            reverse(path.begin(), path.end()); 
            //printf("%c: %s\n", (char) i, path.c_str()); 
        } 
    }
    
}
/**
* Write to the given FancyOutputStream the sequence of bits coding the given symbol.
* PRECONDITION: build() has been called, to create the coding tree, and initialize root pointer and leaves vector.
*/
void HCTree::encode(unsigned char symbol, FancyOutputStream& out) const{
    std::stack<int> stack; 
    HCNode *node = leaves[(int) symbol]; 
    while(node != root){
        HCNode *parent = node->p;
        int b = (parent->c0 == node) ? 0 : 1; 
        stack.push(b);
        node = parent; 
    }
    while(!stack.empty()){
        //printf("%d", stack.top()); 
        out.write_bit(stack.top());
        stack.pop(); 
    }
}

void clear(HCNode *node){
    if(node == nullptr)
        return; 
    clear(node->c0);
    clear(node->c1); 
    delete node; 
}

HCTree::~HCTree(){
    clear(root); 
}

bool HCTree::is_empty() const{
    return root == nullptr; 
}

void compress_tree(HCTree& tree, const std::vector<int> *freq, FancyOutputStream& out_stream){
    //HEREEEEEE
    if(tree.is_empty())
        return; 
    for(int i = 0; i < freq->size(); i++){
        out_stream.write_int((*freq)[i]); 
        out_stream.flush(); 
    }
}


/*
void serialize_node(HCNode *node, FancyOutputStream& out){
    if(node == nullptr){
        out.write_int(-1); 
        out.write_byte(0);
        out.flush(); 
    }
    else{
        out.write_int(node->count);
        out.write_byte(node->symbol); 
        out.flush(); 
        serialize_node(node->c0, out); 
        serialize_node(node->c1, out); 
    }

}
*/

bool compare_canonical(std::pair<unsigned char, std::string&> A, std::pair<unsigned char, std::string&> B){
    if(A.second.length() == B.second.length())
        return A.first < B.first; 
    return A.second.length() < B.second.length(); 
}
/*
void HCTree::serialize_tree(const std::vector<int> *freq, FancyOutputStream& out) const{
    std::vector<std::pair<unsigned char, std::string&> > list; 
    //Get paths
    for(int i = 0; i < freq.size(); i++){
        if(freq[i]){
            std::string path = ""; 
            HCNode *node = leaves[i]; 
            while(node != root){
                if(node == node->p->c0)
                    path += '0';
                else 
                    path += '1'; 
                node = node->p; 
            }
            std::reverse(path.begin(), path.end()); 
            std::pair<unsigned char, std::string&> pair(i, path);
            list.push_back(pair); 
        }
    }

}
*/

void serialize_node(HCNode *node, FancyOutputStream& out){
    if(node == nullptr)
        return;  
    if(node->c0 == nullptr && node->c1 == nullptr){
        out.write_byte(1);
        out.flush(); 
        out.write_byte(node->symbol); 
        out.flush(); 
    }
    else{
        out.write_byte(0); 
        out.flush(); 
    }
    serialize_node(node->c0, out);
    serialize_node(node->c1, out); 
}

void HCTree::serialize_tree(const std::vector<int> *freq, FancyOutputStream& out) const{
    if(root != nullptr){
        printf("SIZE: %d\n", size); 
        printf("CHARACTER COUNT: %d\n", root->count); 
        out.write_int(size); 
        out.flush(); 
        out.write_int(root->count); 
        out.flush(); 
        serialize_node(root, out); 
    }
} 

HCNode* deserialize_node(FancyInputStream& in_stream, int& iter){
    if(!iter)
        return nullptr;
    --iter; 
    HCNode *node;
    if(in_stream.read_byte()){
        unsigned char c = in_stream.read_byte();
        printf("%c\n", c);
        node = new HCNode(0, c); 
        return node; 
    }
    else
        node = new HCNode(-1, 0); 
    node->c0 = deserialize_node(in_stream, iter);
    if(node->c0 != nullptr)
        node->c0->p = node; 
    node->c1 = deserialize_node(in_stream, iter);  
    if(node->c1 != nullptr)
        node->c1->p = node;
    return node; 
}

int HCTree::deserialize_tree(FancyInputStream& in_stream){
    int character_count = 0; 
    //printf("SDFDSFDSFSFD\n"); 
    if(in_stream.filesize()){
        //printf("SDFDSFDSFSFD\n"); 
        size = in_stream.read_int(); 
        character_count = in_stream.read_int(); 
        //printf("SIZE: %d\n", size); 
        //printf("CHARACTER COUNT: %d\n", character_count); 
        int iter = size; 
        root = deserialize_node(in_stream, iter);   
    }
    return character_count; 
}


/*
void output(HCTree& tree, const std::vector<int> *freq, const char *file_name, const char *output_file){
        printf("AA\n");

    FancyInputStream in_stream(file_name); 
    FancyOutputStream out_stream(output_file); 
    // Compress tree 
    compress_tree(tree, freq, out_stream); 

    // Compress data
    for(int cb = in_stream.read_byte(); cb != -1; cb = in_stream.read_byte()){
        tree.encode(cb, out_stream); 
    }

    in_stream.reset(); 
    out_stream.flush(); 
}

/*
HCNode* deserialize_node(FancyInputStream& in_stream, int& iter, int& character_count){
    if(!iter)
        return nullptr;
    int freq = in_stream.read_int();
    unsigned char symbol = in_stream.read_byte(); 

    if(freq == -1)
        return nullptr; 

    HCNode *node = new HCNode(freq, symbol); 
    character_count += freq; 
    //printf("%c: %d\n", symbol, freq); 
    iter--; 
    node->c0 = deserialize_node(in_stream, iter, character_count);

    if(node->c0 != nullptr)
        node->c0->p = node; 

    node->c1 = deserialize_node(in_stream, iter, character_count);  

    if(node->c1 != nullptr)
        node->c1->p = node;

    return node; 
}

int HCTree::deserialize_tree(FancyInputStream& in_stream){
    int character_count = 0; 
    //printf("SDFDSFDSFSFD\n"); 
    if(in_stream.filesize()){
        //printf("SDFDSFDSFSFD\n"); 
        size = in_stream.read_int(); 
        //printf("SIZE: %d\n", size); 
        int iter = size; 
        root = deserialize_node(in_stream, iter, character_count);   
        //read remaining 2 leaves
        in_stream.read_int();
        in_stream.read_byte();
        in_stream.read_int();
        in_stream.read_byte();
        return root->count; 
    }
    return character_count; 
}
*/

/*
void output(HCTree& tree, const std::vector<int> *freq, const char *file_name, const char *output_file){
        printf("AA\n");

    FancyInputStream in_stream(file_name); 
    FancyOutputStream out_stream(output_file); 
    // Compress tree 
    compress_tree(tree, freq, out_stream); 

    // Compress data
    for(int cb = in_stream.read_byte(); cb != -1; cb = in_stream.read_byte()){
        tree.encode(cb, out_stream); 
    }

    in_stream.reset(); 
    out_stream.flush(); 
}
*/

void output(HCTree& tree, const std::vector<int> *freq, const char *file_name, const char *output_file){
        //printf("AA\n");

    FancyInputStream in_stream(file_name); 
    FancyOutputStream out_stream(output_file); 
    // Compress tree 
    tree.serialize_tree(freq, out_stream);   

    // Compress data
    for(int cb = in_stream.read_byte(); cb != -1; cb = in_stream.read_byte()){
        tree.encode(cb, out_stream); 
    }

    in_stream.reset(); 
    out_stream.flush(); 
}

unsigned char HCTree::decode(FancyInputStream& in_stream) const{
    HCNode *node = root; 
    while(node->c0 != nullptr && node->c1 != nullptr){
        int cb = in_stream.read_bit(); 
        node = (cb) ? node->c1 : node->c0; 
    }
    //printf(": %c\n", node->symbol); 
    return node->symbol; 
}

void uncompress(const char *compressed_file, const char *output_file){
    //printf("hi!\n"); 
    FancyInputStream in_stream(compressed_file); 
    FancyOutputStream out_stream(output_file);
    HCTree tree; 
    int character_count = tree.deserialize_tree(in_stream);
    //printf("CHARACTER COUNT: %d\n",character_count); 
    std::string result = ""; 
    while(in_stream.good() && character_count > 0){
        unsigned char c = tree.decode(in_stream); 
        result += c; 
        out_stream.write_byte(c); 
        out_stream.flush(); 
        character_count--; 
    }
    
    //out_stream.write_byte('\n');
    //out_stream.flush(); 
    //printf("%s\n", result.c_str()); 
}

/*
void uncompress(const char *compressed_file, const char *output_file){
    printf("hi!\n"); 
    FancyInputStream in_stream(compressed_file); 
    FancyOutputStream out_stream(output_file);
    std::vector<int> freq(256, 0); 
    unsigned int character_count = 0; 
    for(int i = 0; i < freq.size(); i++){
        
        freq[i] = in_stream.read_int(); 
        character_count += freq[i]; 
        

        //HEREEEEEEE
        if(!in_stream.filesize())
            freq[i] = 0;
        else{
            freq[i] = in_stream.read_int(); 
            character_count += freq[i]; 
        }
    }
    
    printf("CHARACTER COUNT: %d\n",character_count); 
    HCTree tree; 
    tree.build(freq); 
    std::string result = "";         
    printf("AAAAA\n");

    while(in_stream.good() && character_count > 0){
        unsigned char c = tree.decode(in_stream); 
        result += c; 
        out_stream.write_byte(c); 
        out_stream.flush(); 
        character_count--; 
    }
    
    //out_stream.write_byte('\n');
    //out_stream.flush(); 
    //printf("%s\n", result.c_str()); 
}
*/


