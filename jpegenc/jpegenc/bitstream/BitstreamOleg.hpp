#ifndef JPEG_ENCODER_BITSTREAMOLEG_H
#define JPEG_ENCODER_BITSTREAMOLEG_H

#include <vector>

typedef unsigned char BitChar;

// Constants
#define SHIFT_BLOCK  3
#define SHIFT_PAGE  22 // 19+3
const size_t MASK_BYTE   = 0x7;      // = 00000111
const size_t MASK_BLOCK  = 0x3FFFF8; // = 1..11000 = (2^19 -1)<<3
const size_t BLOCK_SIZE  = 0x80000;  // = 524288   =  2^19


class BitstreamOleg {

	int allocatedPages = 1;
	size_t index = 0; // 32bit: 10:19:3
	std::vector<BitChar *> blocks;
	bool memoryChanged = false;

public:
	BitstreamOleg() {
		blocks.push_back(new BitChar[BLOCK_SIZE]);
	}
	
	void add( const bool bit );
	void add( const BitChar byte, short amount );
	bool read( const size_t idx ); // returns the bit
	void print( const bool onlyCurrentPage = false );
	void saveToFile( const char *pathToFile );
private:
	BitChar* getByte( const size_t idx );
	unsigned short fillup( const bool fillWithOnes = true ); // returns how many bits were filled
	void deleteBits( const unsigned int amount );
	void printPage( const size_t page, size_t truncate = BLOCK_SIZE );
	void printByte( const BitChar &byte );
};

#endif


