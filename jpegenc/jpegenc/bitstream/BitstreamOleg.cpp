#include <iostream>
#include <fstream>
#include "BitstreamOleg.hpp"

void BitstreamOleg::saveToFile( const char *pathToFile ) {
	std::ofstream outputStream;
	outputStream.open(pathToFile);
	
	int bitsFilled = fillup(1); // complete the last byte
	
	size_t page = 0;
	size_t pageCount = index >> SHIFT_PAGE; // is actually the index, not count
	
	for (; page <= pageCount; page++) {
		BitChar *a = &blocks[page][0];
		
		size_t bytesOnPage = BLOCK_SIZE;
		if (page == pageCount) // save last page only partly
			bytesOnPage = (index & MASK_BLOCK) >> SHIFT_BLOCK;
		
		while (bytesOnPage--)
			outputStream << *(a++); // move char pointer to the next char
	}
	
	outputStream.close();
	deleteBits(bitsFilled); // restore previous state
}


//  ---------------------------------------------------------------
// |
// |  Read
// |
//  ---------------------------------------------------------------

BitChar* BitstreamOleg::getByte( const size_t idx ) {
	// see if the next char is pointing to a new page
	if (allocatedPages <= ((index + 8) >> SHIFT_PAGE)) {
		blocks.push_back( new BitChar[BLOCK_SIZE] );
		++allocatedPages;
		memoryChanged = true;
	}
	// find the current block and return the correct byte
	return &blocks[ index >> SHIFT_PAGE ][ (index & MASK_BLOCK) >> SHIFT_BLOCK ];
}

bool BitstreamOleg::read( const size_t idx ){
	BitChar* a = getByte(idx);
	short selectedBit = index & MASK_BYTE;
	return (*a >> (7 - selectedBit)) & 1; // shift selected bit to lowest position
}

// TODO: Byte Reader mit (&a)++ und page break

//  ---------------------------------------------------------------
// |
// |  Write
// |
//  ---------------------------------------------------------------

void BitstreamOleg::add( const bool bit ) {
	BitChar* a = getByte(index);
	*a <<= 1;
	if (bit)
		*a |= 1;
	++index; // stupid thing cannot be done inside a function call
}

void BitstreamOleg::add( const BitChar byte, short amount ) {
	BitChar* a = getByte(index);
	short bitIndex = index & MASK_BYTE;
	
	index += amount;
	
	// perfect match. just copy the byte
	if (bitIndex == 0 && amount == 8) {
		*a = byte;
		return;
	}
	
	// calculate if we have to split the char in two
	short overflow = bitIndex + amount - 8;
	if (overflow > 0)
		amount -= overflow;
	
	// append to the current char index
	*a <<= amount;
	*a |= (byte >> (8 - amount)); // shift the upmost bits down, then append bits
	
	// if necessary append to the next one too
	if (overflow > 0) {
		if (memoryChanged) {
			index -= overflow;
			add( (byte >> (8 - amount - overflow)) << (8 - overflow), overflow );
			memoryChanged = false;
		} else {
			// goto next char;  append trimmed overflow bits;  go back to current char
			*(++a)-- |= (byte << amount) >> (8 - overflow);
		}
	}
}

unsigned short BitstreamOleg::fillup( const bool fillWithOnes ) {
	short missingBits = 7 - ((index - 1) & MASK_BYTE);
	if (missingBits == 0)
		return 0; // no need to fill
	
	BitChar* a = getByte(index);
	index += missingBits;
	
	for (int i = missingBits; i>0; i--) { // for the last underfull byte append x bits
		*a <<= 1;
		if (fillWithOnes)
			*a |= 1;
	}
	return missingBits;
}

void BitstreamOleg::deleteBits( const unsigned int amount ) {
	short bitIndexLastChar = index & MASK_BYTE;
	index -= amount;
	BitChar *first = getByte(index); // we only care about the first char since the rest will be overwritten anyway
	
	if (amount <= bitIndexLastChar)
		*first >>= amount; // is actually the last char
	else
		*first >>= ((amount - bitIndexLastChar) % 8); // subtract current bit index and all multiple of 8
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
	else // print all filled blocks
		for (; page < pageCount; page++)
			printPage(page);
	
	// print the current page
	size_t bytesOnLastPage = (index & MASK_BLOCK) >> SHIFT_BLOCK;
	if (index % 8)
		++bytesOnLastPage;
	// else: last bit is exactly 8 bit filled
	
	printPage(page, bytesOnLastPage);
}

void BitstreamOleg::printPage( const size_t page, size_t truncate ) {
	std::cout << "Page [" << page << "]:" << std::endl;
	BitChar *a = &blocks[page][0];
	while (truncate--) {
		printByte(*(a++)); // move char pointer to the next char
		std::cout << " ";
		if (truncate%8 == 0) // new line after 8 bytes (64bit)
			std::cout << std::endl;
	}
	std::cout << std::endl;
}

void BitstreamOleg::printByte( const BitChar &byte ) {
	std::cout
	<< ((byte&0b10000000)>>7) // not pretty but efficient
	<< ((byte&0b01000000)>>6)
	<< ((byte&0b00100000)>>5)
	<< ((byte&0b00010000)>>4)
	<< ((byte&0b00001000)>>3)
	<< ((byte&0b00000100)>>2)
	<< ((byte&0b00000010)>>1)
	<< ((byte&0b00000001));
}
