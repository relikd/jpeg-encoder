#ifndef JPEG_ENCODER_BITSTREAMOLEG_H
#define JPEG_ENCODER_BITSTREAMOLEG_H

#include <vector>

typedef unsigned int BitChar;

const unsigned int BITS_MASK[] {
	0, 1, 3, 7, 15, 31, 63, 127, 255, 511, 1023, 2047, 4095, 8191, 16383, 32767,
	65535, 131071, 262143, 524287, 1048575, 2097151, 4194303, 8388607, 16777215,
	33554431, 67108863, 134217727, 268435455, 536870911, 1073741823, 2147483647,
	4294967295
};

// Constants
#define BITS_FOR_PAGE 17
const unsigned short BITCHAR_SIZE      = sizeof(BitChar);
const unsigned short BITS_PER_BITCHAR  = BITCHAR_SIZE * 8;
const unsigned short MAX_BITCHAR_INDEX = BITS_PER_BITCHAR - 1; // == MASK_BYTE

const unsigned short SHIFT_BLOCK = 3 + (BITCHAR_SIZE / 2);          // 3 = 8bit, 4 = 16bit, 5 = 32bit
const unsigned short SHIFT_PAGE  = SHIFT_BLOCK + BITS_FOR_PAGE;     // bitshift amount to get a page
const size_t MASK_BYTE   = BITS_MASK[SHIFT_BLOCK];                  // bitmask defining a byte    = 0..00111
const size_t MASK_BLOCK  = BITS_MASK[BITS_FOR_PAGE] << SHIFT_BLOCK; // = (2^19 -1)<<3  = 1..11000
const size_t BLOCK_SIZE  = BITS_MASK[BITS_FOR_PAGE] + 1;            // = 2^19 = 524288

// 2^(BITS_FOR_PAGE) * sizeof(BitChar) = 524288 bytes per page


class BitstreamOleg {
	
	int allocatedPages = 1;
	size_t index = 0; // 32bit: 10:17:5
	std::vector<BitChar *> blocks;
	bool memoryChanged = false;
	
public:
	BitstreamOleg() {
		blocks.push_back(new BitChar[BLOCK_SIZE]);
	}
	
	size_t numberOfBits() { return index; }
	
	void add( const bool bit );
	void add( const BitChar byte, unsigned short amount );
	bool read( const size_t idx ); // returns the bit
	void print( const bool onlyCurrentPage = false );
	void saveToFile( const char *pathToFile );
private:
	inline BitChar* getByte( const size_t idx );
	unsigned short fillup( const bool fillWithOnes = true ); // returns how many bits were filled
	void deleteBits( const size_t amount );
	void printPage( const size_t page, size_t truncate = BLOCK_SIZE );
	void printByte( const BitChar &byte );
	void toCString( const BitChar &byte, char* &out );
};

#endif


