#ifndef HELPER
#define HELPER
#include <fstream>
#include <iostream>
using namespace std;

/**
 * Conveniently crash with error messages
 */
void error(const char* message);

/**
 * Handle reading from a file. You must never call read_byte or read_int after calling read_bit!!!
 */
class FancyInputStream {
    private:
        // member variables (aka instance variables)
        const char* FILENAME; // input file's name
        ifstream input_file;  // input stream from which to read
        unsigned char buffer; // bitwise buffer (holds 8 bits = 1 byte)
        int buffer_index;     // current index of bitwise buffer

    public:
        /**
         * Constructor, which initializes a FancyInputStream object to read from the given file
         */
        FancyInputStream(const char* filename);

        /**
         * Returns true if none of the stream's error state flags (eofbit, failbit and badbit) is set.
         * See: https://www.cplusplus.com/reference/ios/ios/good/
         */
        bool good() const;

        /**
         * Return the size of the input file
         */
        int filesize() const;

        /**
         * Move back to the beginning of the input file and clear bitwise buffer
         */
        void reset();

        /**
         * Read a single (usually 4-byte) integer from the file,
         * or crash if there are not enough bytes left in the file
         */
        int read_int();

        /**
         * Read a single byte from the file as an int in the range [0,255],
         * or return -1 if we've reached the end of the file and no more bytes exist to read
         */
        int read_byte();

        /**
         * Read a single bit from the file as an int that is either 0 or 1,
         * or return -1 if we've reached the end of the file and no more bits exist to read
         */
        int read_bit();
};

/**
 * Handle writing to a file. You must never call write_byte or write_int after calling write_bit!!!
 */
class FancyOutputStream {
    private:
        // member variables (aka instance variables)
        ofstream output_file; // output stream to which to write
        unsigned char buffer; // bitwise buffer (holds 8 bits = 1 byte)
        int buffer_index;     // current index of bitwise buffer

    public:
        /**
         * Constructor, which initializes a FancyOutputStream object to write to the given file
         */
        FancyOutputStream(const char* filename);

        /**
         * Destructor, which flushes everything
         */
        ~FancyOutputStream();

        /**
         * Returns true if none of the stream's error state flags (eofbit, failbit and badbit) is set.
         * See: https://www.cplusplus.com/reference/ios/ios/good/
         */
        bool good() const;

        /**
         * Write a single (usually 4-byte) integer to the file
         */
        void write_int(int const & num);

        /**
         * Write a single byte to the file
         */
        void write_byte(unsigned char const & byte);

        /**
         * Write a single bit to the file
         */
        void write_bit(int bit);

        /**
         * Flush the bitwise buffer to the ofstream
         */
        void flush_bitwise();

        /**
         * Flush everything to the file
         */
        void flush();
};

/**
 * Represent nodes in an HCTree (Huffman Tree) object
 */
class HCNode {
    public:
        // member variables (aka instance variables)
        int count;            // count of this node
        unsigned char symbol; // symbol of this node
        HCNode* c0;           // pointer to '0' child
        HCNode* c1;           // pointer to '1' child
        HCNode* p;            // pointer to parent

        /**
         * Constructor, which initializes an HCNode object with a given count and symbol
         */
        HCNode(int count, unsigned char symbol);
};

/**
 * A 'function class' for use as the Compare class in a priority_queue<HCNode*>.
 */
class HCNodePtrComp {
    public:
        bool operator()(HCNode*& lhs, HCNode*& rhs) const;
};
#endif // HELPER
