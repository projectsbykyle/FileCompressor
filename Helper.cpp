#include "Helper.hpp"

// error function implementation
void error(const char* message) {
    cerr << "ERROR: " << message << endl; exit(1);
}

// FancyInputStream function implementations
FancyInputStream::FancyInputStream(const char* filename) : FILENAME(filename), input_file(ifstream(filename,ios::binary)), buffer(0), buffer_index(8) {}

bool FancyInputStream::good() const {
    return input_file.good();
}

int FancyInputStream::filesize() const  {
    return ifstream(FILENAME, ios::ate | ios::binary).tellg();
}

void FancyInputStream::reset() {
    input_file.clear();  // clear EOF flag
    input_file.seekg(0); // move to begining of file
    buffer = 0;          // clear bitwise buffer
    buffer_index = 8;    // move bitwise buffer index back to beginning
}

int FancyInputStream::read_int() {
    if(buffer_index != 8) {
        error("Attempting to read int when bitwise buffer is not empty");
    }
    unsigned int num;                          // temporary variable to store the number
    input_file.read((char*)&num, sizeof(num)); // read the number and store it in 'num'
    if(input_file.eof()) {                     // not enough bytes in the file to read it
        error("Not enough bytes to read the next int");
    }
    return num;
}

int FancyInputStream::read_byte() {
    return input_file.get();
}

int FancyInputStream::read_bit() {
    // if there are no more bits to read in the buffer,
    if(buffer_index == 8) {
        int const & tmp = read_byte(); // try to read the next byte

        // if there are no more bytes to read, there are no more bits to read
        if(tmp == -1) {
            return -1;
        }

        // we read a byte successfully, so update our buffer
        buffer = tmp;
        buffer_index = 0;
    }

    // read the next bit from the bitwise buffer
    return (buffer >> (7-buffer_index++)) & 1;
}

// FancyOutputStream function implementations
FancyOutputStream::FancyOutputStream(const char* filename) : output_file(ofstream(filename,ios::binary)), buffer(0), buffer_index(0) {}

FancyOutputStream::~FancyOutputStream() {
    flush();
}

bool FancyOutputStream::good() const {
    return output_file.good();
}

void FancyOutputStream::write_int(int const & num) {
    if(buffer_index != 0) {
        error("Attempting to write int when bitwise buffer is not empty");
    }
    output_file.write((char*)&num, sizeof(num)); // write 'num' to file
}

void FancyOutputStream::write_byte(unsigned char const & byte) {
    if(buffer_index != 0) {
        error("Attempting to write byte when bitwise buffer is not empty");
    }
    output_file.put(byte);
}

void FancyOutputStream::write_bit(int bit) {
    // crash if invalid input
    if(bit != 0 && bit != 1) {
        error("Trying to write invalid bit");
    }

    // add bit to bitwise buffer
    buffer |= (bit << (7-buffer_index++));

    // if the bitwise buffer is full,
    if(buffer_index == 8) {
        flush_bitwise(); // flush it
    }
}

void FancyOutputStream::flush_bitwise() {
    // if we have bits in our bitwise buffer,
    if(buffer_index != 0) {
        output_file.put(buffer); // write the bitwise buffer to the ofstream
        buffer = 0;              // reset the buffer
        buffer_index = 0;        // reset the buffer index
    }
}

void FancyOutputStream::flush() {
    flush_bitwise();     // try to flush the bitwise buffer
    output_file.flush(); // flush the ofstream
}

// HCNode function implementations
HCNode::HCNode(int count, unsigned char symbol) : count(count), symbol(symbol), c0(nullptr), c1(nullptr), p(nullptr) {}

bool HCNodePtrComp::operator()(HCNode*& lhs, HCNode*& rhs) const {
    // if the counts are different, compare counts
    if(lhs->count != rhs->count){
        return lhs->count > rhs->count;
    }

    // if the counts are equal, use symbol to break tie
    return lhs->symbol > rhs->symbol;
}
