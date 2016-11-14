#ifndef JPEG_ENCODER_BITSTREAMOLEG_H
#define JPEG_ENCODER_BITSTREAMOLEG_H

#include <vector>

typedef unsigned long BitChar;

const unsigned long BITS_MASK[] { // 0000, 0001, 0011, 0111, 1111, ...
	0x0, 0x1, 0x3, 0x7, 0xF, 0x1F, 0x3F, 0x7F, 0xFF, 0x1FF, 0x3FF, 0x7FF, 0xFFF, 0x1FFF,
	0x3FFF, 0x7FFF, 0xFFFF, 0x1FFFF, 0x3FFFF, 0x7FFFF, 0xFFFFF, 0x1FFFFF,0x3FFFFF, 0x7FFFFF,
	0xFFFFFF, 0x1FFFFFF, 0x3FFFFFF, 0x7FFFFFF, 0xFFFFFFF, 0x1FFFFFFF, 0x3FFFFFFF, 0x7FFFFFFF,
	0xFFFFFFFF, 0x1FFFFFFFF, 0x3FFFFFFFF, 0x7FFFFFFFF, 0xFFFFFFFFF, 0x1FFFFFFFFF, 0x3FFFFFFFFF,
	0x7FFFFFFFFF, 0xFFFFFFFFFF, 0x1FFFFFFFFFF, 0x3FFFFFFFFFF, 0x7FFFFFFFFFF, 0xFFFFFFFFFFF,
	0x1FFFFFFFFFFF, 0x3FFFFFFFFFFF, 0x7FFFFFFFFFFF, 0xFFFFFFFFFFFF, 0x1FFFFFFFFFFFF,
	0x3FFFFFFFFFFFF, 0x7FFFFFFFFFFFF, 0xFFFFFFFFFFFFF, 0x1FFFFFFFFFFFFF, 0x3FFFFFFFFFFFFF,
	0x7FFFFFFFFFFFFF, 0xFFFFFFFFFFFFFF, 0x1FFFFFFFFFFFFFF, 0x3FFFFFFFFFFFFFF, 0x7FFFFFFFFFFFFFF,
	0xFFFFFFFFFFFFFFF, 0x1FFFFFFFFFFFFFFF, 0x3FFFFFFFFFFFFFFF, 0x7FFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF
};

// Constants
#define BITS_FOR_PAGE 17
const unsigned short BITCHAR_SIZE      = sizeof(BitChar);
const unsigned short BITS_PER_BITCHAR  = BITCHAR_SIZE * 8;
const unsigned short MAX_BITCHAR_INDEX = BITS_PER_BITCHAR - 1; // == MASK_BYTE

const unsigned short SHIFT_BLOCK = 6; // 3 = 8bit, 4 = 16bit, 5 = 32bit, 6 = 64bit
const unsigned short SHIFT_PAGE  = SHIFT_BLOCK + BITS_FOR_PAGE;     // bitshift amount to get a page
const size_t MASK_BYTE   = BITS_MASK[SHIFT_BLOCK];                  // bitmask defining a byte    = 0..00111
const size_t MASK_BLOCK  = BITS_MASK[BITS_FOR_PAGE] << SHIFT_BLOCK; // = (2^19 -1)<<3  = 1..11000
const size_t BLOCK_SIZE  = BITS_MASK[BITS_FOR_PAGE] + 1;            // = 2^19 = 524288

// 2^(BITS_FOR_PAGE) * sizeof(BitChar) = 524288 bytes per page


class BitstreamOleg {
	
	size_t allocatedPages = 0;
	std::vector<BitChar *> blocks;
	BitChar *currentChar;
	
public:
	BitstreamOleg( size_t preAllocPages = 1 ) : allocatedPages(preAllocPages) {
		while (preAllocPages--) // dont be stupud and call the constructor with 0
			blocks.push_back(new BitChar[BLOCK_SIZE]);
		currentChar = &blocks[0][0];
	}
	
	size_t numberOfBits() {
		return (pageIndex << SHIFT_PAGE) + (byteIndex << SHIFT_BLOCK) + bitIndex;
	}
	
	// public access
	void add( const bool bit );
	void add( const BitChar byte, unsigned short amount );
	bool read( const size_t idx ); // returns the bit
	void print( const bool onlyCurrentPage = false );
	void saveToFile( const char *pathToFile );
	
private:
	// bitstream logic
	unsigned short fillup( const bool fillWithOnes = true ); // returns how many bits were filled
	void deleteBits( const size_t amount );
	void printPage( const size_t page, size_t truncate = BLOCK_SIZE );
	void printByte( const BitChar &byte );
	inline void mapBitCharToChar( const BitChar &in, char* &out );
	
	// handle indexing
	size_t bitIndex = 0;
	size_t byteIndex = 0;
	size_t pageIndex = 0;
	inline void appendPage();
	inline void appendByte();
	inline void upCountBit();
	inline void upCountBits( const unsigned short amount );
	inline void downCountBits( const size_t amount );
};

#endif


