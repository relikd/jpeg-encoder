//#include <iostream>
//#include <fstream>
#include "BitstreamOleg.hpp"


//  ---------------------------------------------------------------
// |
// |  Indexing and BitChar Pointer Management
// |
//  ---------------------------------------------------------------

inline void BitstreamOleg::appendPage() {
	++pageIndex;
	if (allocatedPages <= pageIndex) {
		// create new page
		blocks.push_back( new BitChar[BLOCK_SIZE] );
		++allocatedPages;
	}
}

inline void BitstreamOleg::appendByte() {
	if (++byteIndex >= BLOCK_SIZE) {
		byteIndex = 0;
		appendPage();
		currentChar = &blocks[pageIndex][0];
	} else {
		currentChar++;
	}
}

inline void BitstreamOleg::upCountBit() {
	if (++bitIndex >= BITS_PER_BITCHAR) {
		bitIndex = 0;
		appendByte();
	}
}

inline void BitstreamOleg::upCountBits( const unsigned short amount ) {
	bitIndex += amount;
	if (bitIndex >= BITS_PER_BITCHAR) {
		bitIndex %= BITS_PER_BITCHAR;
		appendByte();
	}
}

inline void BitstreamOleg::downCountBits( const size_t amount ) {
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
	byteIndex = (singleIndex & MASK_BLOCK) >> SHIFT_BLOCK;
	bitIndex  = (singleIndex & MASK_BYTE);
	
	// move char to new position
	currentChar = &blocks[ pageIndex ][ byteIndex ];
}

//  ---------------------------------------------------------------
// |
// |  Read
// |
//  ---------------------------------------------------------------

bool BitstreamOleg::read( const size_t idx ){
	BitChar* a = &blocks[ idx >> SHIFT_PAGE ][ (idx & MASK_BLOCK) >> SHIFT_BLOCK ];
	unsigned short selectedBit = idx & MASK_BYTE;
	return (*a >> (MAX_BITCHAR_INDEX - selectedBit)) & 1; // shift selected bit to lowest position
}

//  ---------------------------------------------------------------
// |
// |  Write
// |
//  ---------------------------------------------------------------

void BitstreamOleg::add( const bool bit ) {
	*currentChar = (*currentChar << 1) | bit;
	upCountBit();
}

void BitstreamOleg::add( const BitChar byte, unsigned short amount ) {
	// calculate if we have to split the char in two
	short overflow = bitIndex + amount - BITS_PER_BITCHAR;
	
	if (overflow > 0) {
		// write current BitChar
		amount -= overflow;
		*currentChar = (*currentChar << amount) | ((byte >> overflow) & BITS_MASK[amount]);
		upCountBits(amount);
		// and append to the next one
		*currentChar = (byte & BITS_MASK[overflow]);
		upCountBits(overflow);
	} else {
		// everything fits into the current BitChar
		*currentChar = (*currentChar << amount) | (byte & BITS_MASK[amount]);
		upCountBits(amount);
	}
}

unsigned short BitstreamOleg::fillup( const bool fillWithOnes ) {
	if (bitIndex == 0)
		return 0; // no need to fill
	
	short missingBits = BITS_PER_BITCHAR - bitIndex;
	
	*currentChar <<= missingBits; // for the last underfull byte append x bits
	if (fillWithOnes)
		*currentChar |= (1 << missingBits) - 1;
	
	upCountBits(missingBits);
	return missingBits;
}

void BitstreamOleg::deleteBits( const size_t amount ) {
	short bitIndexBefore = bitIndex;
	downCountBits(amount);
	// we only care about the first char since the rest will be overwritten anyway
	if (amount <= bitIndexBefore)
		*currentChar >>= amount; // is actually the last char
	else
		*currentChar >>= (amount - bitIndexBefore) % BITS_PER_BITCHAR; // subtract current bit index and all multiples of BitChar size
}

//  ---------------------------------------------------------------
// |
// |  Print
// |
//  ---------------------------------------------------------------

void BitstreamOleg::print( const bool onlyCurrentPage ) {
	size_t bytesOnLastPage = byteIndex;
	if (bitIndex % BITS_PER_BITCHAR)
		++bytesOnLastPage;
	// else: last byte is exactly 8 bit filled
	
	for (size_t page = 0; page <= pageIndex; page++) {
		if (page == pageIndex)
			printPage(page, bytesOnLastPage); // print the current page
		
		else if (onlyCurrentPage == false)
			printPage(page); // print all completely filled blocks
	}
}

// new line after 8 bytes (64bit)
const unsigned short breakByteAfter = 8 / BITCHAR_SIZE;

void BitstreamOleg::printPage( const size_t page, size_t truncate ) {
	printf("Page [%d]\n", (int)page);
	BitChar *a = &blocks[page][0];
	while (truncate--) {
		printByte(*(a++)); // move char pointer to the next char
		if (truncate % breakByteAfter == 0)
			printf("\n");
	}
	printf("\n");
}

void BitstreamOleg::printByte( const BitChar &byte ) {
	unsigned short idx = BITS_PER_BITCHAR;
	while (idx--) {
		printf("%d", (bool)((byte >> idx) & 1)); // print bit at index 'idx'
		if (idx % 8 == 0)
			printf(" "); // space after 8bit / one byte
	}
}

//  ---------------------------------------------------------------
// |
// |  Save file
// |
//  ---------------------------------------------------------------

void BitstreamOleg::saveToFile( const char *pathToFile ) {
	FILE *f = fopen(pathToFile, "w");
	char *byteRemap = new char[BITCHAR_SIZE]; // needed to correct the byte order for int
	
	int bitsFilled = fillup(1); // complete the last byte
	
	size_t bytesOnLastPage = byteIndex;
	if (bytesOnLastPage > 0)
		bytesOnLastPage -= 1;// -1 = process last BitChar separately
	
	
	for (size_t page = 0; page <= pageIndex; page++) {
		size_t bytesOnPage = BLOCK_SIZE;
		if (page == pageIndex)
			bytesOnPage = bytesOnLastPage; // save last page only partly
		
		BitChar *a = &blocks[page][0];
		
		// save values to binary file
		while (bytesOnPage--) {
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

inline void BitstreamOleg::mapBitCharToChar( const BitChar &in, char* &out ) {
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

