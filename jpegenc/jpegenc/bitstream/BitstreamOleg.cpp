#include <iostream>
#include <fstream>
#include "BitstreamOleg.hpp"

//  ---------------------------------------------------------------
// |
// |  Read
// |
//  ---------------------------------------------------------------

inline BitChar* BitstreamOleg::getByte( const size_t idx ) {
	// see if the next char is pointing to a new page
	if (allocatedPages <= ((idx + BITS_PER_BITCHAR) >> SHIFT_PAGE)) {
		blocks.push_back( new BitChar[BLOCK_SIZE] );
		++allocatedPages;
		memoryChanged = true;
	}
	// find the current block and return the correct byte
	return &blocks[ idx >> SHIFT_PAGE ][ (idx & MASK_BLOCK) >> SHIFT_BLOCK ];
}

bool BitstreamOleg::read( const size_t idx ){
	BitChar* a = getByte(idx);
	unsigned short selectedBit = idx & MASK_BYTE;
	return (*a >> (MAX_BITCHAR_INDEX - selectedBit)) & 1; // shift selected bit to lowest position
}

// TODO: Byte Reader mit (&a)++ und page break

//  ---------------------------------------------------------------
// |
// |  Write
// |
//  ---------------------------------------------------------------

void BitstreamOleg::add( const bool bit ) {
	BitChar* a = getByte(index++);
	*a = (*a << 1) | bit;
}

void BitstreamOleg::add( const BitChar byte, unsigned short amount ) {
	BitChar* a = getByte(index);

	// calculate if we have to split the char in two
	short overflow = (index & MASK_BYTE) + amount - BITS_PER_BITCHAR;
	
	index += amount;
	
	if (overflow > 0) {
		// write current BitChar
		amount -= overflow;
		*a <<= amount;
		*a |= (byte >> overflow) & BITS_MASK[amount];
		
		// and append to the next one
		if (memoryChanged) {
			index -= overflow;
			add( byte, overflow );
			memoryChanged = false;
		} else {
			// append trimmed overflow bits to next BitChar
			*(a+1) |= byte & BITS_MASK[overflow];
		}
	}
	else // everything fits into the current BitChar
	{
		*a <<= amount;
		*a |= byte & BITS_MASK[amount];
	}
}

unsigned short BitstreamOleg::fillup( const bool fillWithOnes ) {
	short missingBits = BITS_PER_BITCHAR - (index & MASK_BYTE);
	if (missingBits == BITS_PER_BITCHAR)
		return 0; // no need to fill
	
	BitChar* a = getByte(index);
	index += missingBits;
	
	*a <<= missingBits; // for the last underfull byte append x bits
	if (fillWithOnes)
		*a |= (1 << missingBits) - 1;
	
	return missingBits;
}

void BitstreamOleg::deleteBits( const size_t amount ) {
	if (amount >= index) {
		index = 0;
		*getByte(index) = 0;
		return;
	}
	
	short bitIndexLastChar = index & MASK_BYTE;
	index -= amount;
	BitChar *first = getByte(index); // we only care about the first char since the rest will be overwritten anyway
	
	if (amount <= bitIndexLastChar)
		*first >>= amount; // is actually the last char
	else
		*first >>= (amount - bitIndexLastChar) % BITS_PER_BITCHAR; // subtract current bit index and all multiples of BitChar size
}

//  ---------------------------------------------------------------
// |
// |  Print
// |
//  ---------------------------------------------------------------

void BitstreamOleg::print( const bool onlyCurrentPage ) {
	size_t page = 0;
	size_t pageCount = index >> SHIFT_PAGE; // is actually the index, not count
	
	if (onlyCurrentPage)
		page = pageCount;
	else // print all completely filled blocks
		for (; page < pageCount; page++)
			printPage(page);
	
	// print the current page
	size_t bytesOnLastPage = (index & MASK_BLOCK) >> SHIFT_BLOCK;
	if (index % BITS_PER_BITCHAR)
		++bytesOnLastPage;
	// else: last bit is exactly 8 bit filled
	
	printPage(page, bytesOnLastPage);
}

// new line after 8 bytes (64bit)
const unsigned short breakByteAfter = 8 / BITCHAR_SIZE;

void BitstreamOleg::printPage( const size_t page, size_t truncate ) {
	std::cout << "Page [" << page << "]:" << std::endl;
	BitChar *a = &blocks[page][0];
	while (truncate--) {
		printByte(*(a++)); // move char pointer to the next char
		if (truncate % breakByteAfter == 0)
			std::cout << std::endl;
	}
	std::cout << std::endl;
}

void BitstreamOleg::printByte( const BitChar &byte ) {
	short idx = BITS_PER_BITCHAR;
	while (idx--) {
		std::cout << ((byte >> idx) & 1); // get bit at index 'idx'
		if (idx % 8 == 0)
			std::cout << " "; // space after 8bit / one byte
	}
}

//  ---------------------------------------------------------------
// |
// |  Save file
// |
//  ---------------------------------------------------------------

void BitstreamOleg::saveToFile( const char *pathToFile ) {
	FILE *f = fopen(pathToFile, "w");
	
	int bitsFilled = fillup(1); // complete the last byte
	
	size_t page = 0;
	size_t pageCount = index >> SHIFT_PAGE; // is actually the index, not count
	
	char *byteRemap = new char[BITCHAR_SIZE];
	
	for (; page <= pageCount; page++) {
		BitChar *a = &blocks[page][0];
		
		// how much bytes should be processed on this page
		size_t bytesOnPage = BLOCK_SIZE;
		if (page == pageCount) {// save last page only partly
			bytesOnPage = ((index & MASK_BLOCK) >> SHIFT_BLOCK);
			if (bytesOnPage > 0)
				bytesOnPage -= 1;// -1 = process last BitChar separately
		}
		
		// save values to binary file
		while (bytesOnPage--) {
			toCString(*(a++), byteRemap);
			fwrite(byteRemap, 1, BITCHAR_SIZE, f); // move char pointer to the next char
		}
		
		// get last BitChar separately because it could contain 4 bytes and we only need 1 byte
		if (page == pageCount) {
			toCString(*a, byteRemap);
			fwrite(byteRemap, 1, BITCHAR_SIZE - (bitsFilled / 8), f); // omit trailing 0xFF chars
		}
	}
	
	deleteBits(bitsFilled); // restore previous state
	fclose(f);
}

void BitstreamOleg::toCString( const BitChar &byte, char* &out ) {
	switch (BITCHAR_SIZE) {
		case 4: out[0] = byte>>24; out[1] = byte>>16;  out[2] = byte>>8;  out[3] = byte; break;
		case 2: out[0] = byte>>8;  out[1] = byte; break;
		case 1: out[0] = byte; break;
	}
	// this hack is possible because a char will copy only the 8 least significant bits
}

