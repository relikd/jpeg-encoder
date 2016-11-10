#include <iostream>
#include "BitstreamOleg.hpp"

//  ---------------------------------------------------------------
// |
// |  Read
// |
//  ---------------------------------------------------------------

char* BitstreamOleg::getByte( const size_t idx ) {
	// when index is pointing to the last byte in this block, create a new block
	if ((index & MASK_BLOCK) == MASK_BLOCK) {
		blocks.push_back(new char[BLOCK_SIZE]);
		memoryChanged = true; //
	}
	// find the current block and return the correct byte
	return &blocks[ index>>SHIFT_PAGE ][ (index&MASK_BLOCK)>>SHIFT_BLOCK ];
}

bool BitstreamOleg::read( const size_t idx ){
	char* a = getByte(idx);
	char selectedBit = index & MASK_BYTE;
	return (*a >> (7-selectedBit)) & 1; // shift bit to lowest position
}

// TODO: Byte Reader mit (&a)++ und page break

//  ---------------------------------------------------------------
// |
// |  Write
// |
//  ---------------------------------------------------------------

void BitstreamOleg::add( const bool bit ) {
	char* a = getByte(index++);
	*a <<= 1;
	if (bit)
		*a |= 1;
}

void BitstreamOleg::add( const char bits, int amount ) {
	char* a = getByte(index);
	char bitIndex = index & MASK_BYTE;
	
	index += amount;
	
	// perfect match. just copy the byte
	if (bitIndex == 0 && amount == 8) {
		*a = bits;
		return;
	}
	
	// calculate if we have to split the char in two
	char overflow = bitIndex + amount - 8;
	if (overflow > 0)
		amount -= overflow;
	
	// append to the current char index
	*a <<= amount;
	*a |= (bits >> (8 - amount)); // shift the upmost bits down, then append bits
	
	// if necessary append to the next one too
	if (overflow > 0) {
		if (memoryChanged) {
			index -= overflow;
			add((bits >> (8-amount-overflow)) << (8-overflow), overflow);
			memoryChanged = false;
		} else {
			// goto next char;  append trimmed overflow bits;  go back to current char
			*(++a)-- |= (bits << amount) >> (8-overflow);
		}
	}
}

void BitstreamOleg::fillup( const bool fillWithOnes ) {
	char missingBits = 8 - (index & MASK_BYTE);
	if (missingBits == 0)
		return; // no need to fill
	
	char* a = getByte(index);
	index += missingBits;
	
	while (missingBits--) { // for the last underfull byte append x bits
		*a <<= 1;
		if (fillWithOnes)
			*a |= 1;
	}
}

//  ---------------------------------------------------------------
// |
// |  Print
// |
//  ---------------------------------------------------------------

void BitstreamOleg::print( const bool onlyCurrentPage ) {
	size_t page = 0;
	size_t pageCount = index>>SHIFT_PAGE; // is actually the index, not count
	
	if (onlyCurrentPage)
		page = pageCount;
	else // print all filled blocks
		for (; page < pageCount; page++)
			printPage(page);
	
	// print the current page
	size_t bytesInLastPage = (index&MASK_BLOCK)>>SHIFT_BLOCK;
	if (index % 8)
		++bytesInLastPage;
	// else: last bit is exactly 8 bit filled
	
	printPage(page, bytesInLastPage);
}

void BitstreamOleg::printPage( const size_t page, size_t truncate ) {
	std::cout << "Page ["<<page<<"]:" << std::endl;
	char *a = &blocks[page][0];
	while (truncate--) {
		printByte(*(a++)); // move char pointer to the next char
		std::cout << " ";
		if (truncate%8 == 0) // new line after 8 bytes (64bit)
			std::cout << std::endl;
	}
	std::cout << std::endl;
}

void BitstreamOleg::printByte( const char &byte ) {
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
