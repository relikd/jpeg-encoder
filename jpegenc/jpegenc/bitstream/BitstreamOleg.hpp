#ifndef JPEG_ENCODER_BITSTREAMOLEG_H
#define JPEG_ENCODER_BITSTREAMOLEG_H

#include <vector>

// Constants
#define SHIFT_BLOCK  3
#define SHIFT_PAGE  22 // 19+3
const unsigned long MASK_BYTE   = 0x7;      // = 00000111
const unsigned long MASK_BLOCK  = 0x3FFFF8; // = 1..11000 = (2^19 -1)<<3
const unsigned long BLOCK_SIZE  = 0x80000;  // = 524288   =  2^19


class BitstreamOleg {

	unsigned long index = 0; // 32bit: 10:19:3
	std::vector<char *> blocks;
	bool memoryChanged = false;

public:
	BitstreamOleg() {
		blocks.push_back(new char[BLOCK_SIZE]);
	}

	bool read( const size_t idx );
	void add( const bool bit );
	void add( const char bits, int amount );
	void fillup( const bool fillWithOnes = true );
	void print( const bool onlyCurrentPage = false );
	
private:
	char* getByte( const size_t idx );
	void printPage( const size_t page, size_t truncate = BLOCK_SIZE );
	void printByte( const char &byte );
};

#endif


