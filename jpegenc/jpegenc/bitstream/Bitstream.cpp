//#include <iostream>
//#include <fstream>
#include "Bitstream.hpp"


//  ---------------------------------------------------------------
// |
// |  Indexing and BitChar Pointer Management
// |
//  ---------------------------------------------------------------

inline void Bitstream::appendPage() {
	++pageIndex;
	if (allocatedPages <= pageIndex) {
		// create new page
		blocks.push_back( new BitChar[BLOCK_SIZE] );
		++allocatedPages;
	}
}

inline void Bitstream::appendBitChar() {
	if (++bitCharIndex >= BLOCK_SIZE) {
		bitCharIndex = 0;
		appendPage();
		currentChar = &blocks[pageIndex][0];
	} else {
		currentChar++;
	}
}

inline void Bitstream::upCountBit() {
	if (++bitIndex >= BITS_PER_BITCHAR) {
		bitIndex = 0;
		appendBitChar();
	}
}

inline void Bitstream::upCountBits( const unsigned short amount ) {
	bitIndex += amount;
	if (bitIndex >= BITS_PER_BITCHAR) {
		bitIndex &= MASK_BIT_INDEX; // MODULO: BITS_PER_BITCHAR
		appendBitChar();
	}
}

inline void Bitstream::downCountBits( const size_t amount ) {
	if (amount <= bitIndex) {
		bitIndex -= amount;
		return; // pointer stays on the current BitChar
	}
	
	// combine all three indices to a single one and subtract the amount
	size_t singleIndex = numberOfBits();
	if (amount > singleIndex)
		singleIndex = 0;
	else
		singleIndex -= amount;
	
	// then split the index into three again
	pageIndex = (singleIndex >> SHIFT_PAGE);
	bitCharIndex = (singleIndex & MASK_BITCHAR_INDEX) >> SHIFT_BLOCK;
	bitIndex  = (singleIndex & MASK_BIT_INDEX);
	
	// move char to new position
	currentChar = &blocks[ pageIndex ][ bitCharIndex ];
}

//  ---------------------------------------------------------------
// |
// |  Read
// |
//  ---------------------------------------------------------------

bool Bitstream::read( const size_t idx ){
	BitChar* a = &blocks[ idx >> SHIFT_PAGE ][ (idx & MASK_BITCHAR_INDEX) >> SHIFT_BLOCK ];
	unsigned short selectedBit = idx & MASK_BIT_INDEX;
	return (*a >> (MAX_BITCHAR_INDEX - selectedBit)) & 1; // shift selected bit to lowest position
}

//  ---------------------------------------------------------------
// |
// |  Write
// |
//  ---------------------------------------------------------------

void Bitstream::add( const bool bit ) {
	*currentChar = (*currentChar << 1) | bit;
	upCountBit();
}

void Bitstream::add( const BitChar input, unsigned short amount ) {
	// calculate if we have to split the char in two
	short overflow = bitIndex + amount - BITS_PER_BITCHAR;
	
	if (overflow > 0) {
		// write current BitChar
		amount -= overflow;
		*currentChar = (*currentChar << amount) | ((input >> overflow) & BITS_MASK[amount]);
		upCountBits(amount);
		// and append to the next one
		*currentChar = (input & BITS_MASK[overflow]);
		upCountBits(overflow);
	} else {
		// everything fits into the current BitChar
		*currentChar = (*currentChar << amount) | (input & BITS_MASK[amount]);
		upCountBits(amount);
	}
}

unsigned short Bitstream::fillup( const bool fillWithOnes ) {
	if (bitIndex == 0)
		return 0; // no need to fill
	
	unsigned short missingBits = BITS_PER_BITCHAR - bitIndex;
	
	*currentChar <<= missingBits; // for the last underfull byte append x bits
	if (fillWithOnes)
		*currentChar |= (1 << missingBits) - 1;
	
	upCountBits(missingBits);
	return missingBits;
}

void Bitstream::deleteBits( const size_t amount ) {
	unsigned short bitIndexBefore = bitIndex;
	downCountBits(amount);
	// we only care about the first char since the rest will be overwritten anyway
	if (amount <= bitIndexBefore)
		*currentChar >>= amount; // is actually the last char
	else
		*currentChar >>= (amount - bitIndexBefore) & MASK_BIT_INDEX; // subtract current bit index and all multiples of BitChar size
}

//  ---------------------------------------------------------------
// |
// |  Print
// |
//  ---------------------------------------------------------------

void Bitstream::print( const bool onlyCurrentPage ) {
	for (unsigned short page = 0; page <= pageIndex; page++) {
		if (page == pageIndex) // print the current page
			printPage(page, bitCharIndex + (bitIndex ? 1 : 0)); // omit last byte if no bit set (bitIndex==0)
		
		else if (onlyCurrentPage == false)
			printPage(page); // print all completely filled blocks
	}
}

// new line after 8 bytes (64bit)
const unsigned short breakByteAfter = 8 / BITCHAR_SIZE;

void Bitstream::printPage( const size_t page, size_t truncate ) {
	printf("Page [%d]\n", (int)page);
	BitChar *a = &blocks[page][0];
	while (truncate--) {
		printBitChar(*(a++)); // move char pointer to the next char
		if (truncate % breakByteAfter == 0)
			printf("\n");
	}
	printf("\n");
}

void Bitstream::printBitChar( const BitChar &byte ) {
	unsigned short idx = BITS_PER_BITCHAR;
	while (idx--) {
		printf("%d", (bool)((byte >> idx) & 1)); // print bit at index 'idx'
		if ((idx & 0x7) == 0) // MODULO: 8
			printf(" "); // space after 8bit / one byte
	}
}

//  ---------------------------------------------------------------
// |
// |  Save file
// |
//  ---------------------------------------------------------------

void Bitstream::saveToFile( const char *pathToFile ) {
	FILE *f = fopen(pathToFile, "w");
	char *byteRemap = new char[BITCHAR_SIZE]; // needed to correct the byte order for int
	
	int bitsFilled = fillup(1); // complete the last byte
	
	size_t bitCharsOnLastPage = bitCharIndex;
	if (bitCharsOnLastPage > 0)
		bitCharsOnLastPage -= 1;// -1 = process last BitChar separately
	
	
	for (size_t page = 0; page <= pageIndex; page++) {
		size_t bitCharsOnPage = BLOCK_SIZE;
		if (page == pageIndex)
			bitCharsOnPage = bitCharsOnLastPage; // save last page only partly
		
		BitChar *a = &blocks[page][0];
		
		// save values to binary file
		while (bitCharsOnPage--) {
			mapBitCharToChar(*(a++), byteRemap);
			fwrite(byteRemap, 1, BITCHAR_SIZE, f); // move char pointer to the next char
		}
		
		// get last BitChar separately because it could contain 4 bytes even if we only need 1 byte
		if (page == pageIndex) {
			mapBitCharToChar(*a, byteRemap);
			fwrite(byteRemap, 1, BITCHAR_SIZE - (bitsFilled / 8), f); // omit trailing 0xFF chars
		}
	}
	delete[] byteRemap;
	fclose(f);
	
	deleteBits(bitsFilled); // restore previous state
}

inline void Bitstream::mapBitCharToChar( const BitChar &in, char* &out ) {
	// this bitshift hack is possible because a char will copy only the 8 least significant bits
	switch (BITCHAR_SIZE) {
		case 8: out[0]=in>>56; out[1]=in>>48; out[2]=in>>40; out[3]=in>>32;
			    out[4]=in>>24; out[5]=in>>16; out[6]=in>>8;  out[7]=in; break;
		case 4: out[0]=in>>24; out[1]=in>>16; out[2]=in>>8;  out[3]=in; break;
		case 2: out[0]=in>>8;  out[1]=in; break;
		case 1: out[0]=in; break;
	}
	// not readable, but compact and efficient
}

