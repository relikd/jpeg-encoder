#ifndef JPEG_ENCODER_BITSTREAMOLEG_H
#define JPEG_ENCODER_BITSTREAMOLEG_H

#include <vector>

typedef unsigned long long Word;

const unsigned long long BITS_MASK[] { // 0000, 0001, 0011, 0111, 1111, ...
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
const unsigned short WORD_SIZE      = sizeof(Word);
const unsigned short BITS_PER_WORD  = WORD_SIZE * 8;
const unsigned short MAX_INDEX_WORD = BITS_PER_WORD - 1; // == MASK_BIT_INDEX

const unsigned short SHIFT_WORD = 6; // 3 = 8bit, 4 = 16bit, 5 = 32bit, 6 = 64bit
const unsigned short SHIFT_PAGE = SHIFT_WORD + BITS_FOR_PAGE;     // bitshift amount to get a page
const size_t MASK_BIT_INDEX  = BITS_MASK[SHIFT_WORD];                  // bitmask defining a byte    = 0..00111
const size_t MASK_WORD_INDEX = BITS_MASK[BITS_FOR_PAGE] << SHIFT_WORD; // = (2^17 -1)<<6  = 1..11000000
const size_t PAGE_SIZE       = BITS_MASK[BITS_FOR_PAGE] + 1;            // = 2^17

// 2^(BITS_FOR_PAGE) * sizeof(Word) = 524288 bytes per page


class Bitstream {
	
	size_t allocatedPages = 0;
	std::vector<Word *> book;
	Word *currentWord;
	
public:
	Bitstream( size_t preAllocPages = 1 ) : allocatedPages(preAllocPages) {
		while (preAllocPages--) // dont be stupid and call the constructor with 0
			book.push_back(new Word[PAGE_SIZE]);
		currentWord = &book[0][0];
	}
	
	size_t numberOfBits() {
		return (pageIndex << SHIFT_PAGE) + (WordIndex << SHIFT_WORD) + bitIndex;
	}
	
	// public access
	void add( const bool bit );
	void add( const Word input, unsigned short amount );
	bool read( const size_t idx ); // returns the bit
	void print( const bool onlyCurrentPage = false );
	void saveToFile( const char *pathToFile );

    // public for testing purposes
    unsigned short fillup( const bool fillWithOnes = true ); // returns how many bits were filled

private:
	// bitstream logic
	void deleteBits( const size_t amount );
	void printPage( const size_t page, size_t truncate = PAGE_SIZE );
	void printWord( const Word &input );
	inline void mapWordToChar( const Word &in, char* &out );
	
	// handle indexing
	unsigned short bitIndex = 0;
	size_t WordIndex = 0;
	size_t pageIndex = 0;
	inline void appendPage();
	inline void appendWord();
	inline void upCountBit();
	inline void upCountBits( const unsigned short amount );
	inline void downCountBits( const size_t amount );
};

#endif


