//#include <iostream>
//#include <fstream>
#include "Bitstream.hpp"


//  ---------------------------------------------------------------
// |
// |  Indexing and Word Pointer Management
// |
//  ---------------------------------------------------------------

inline void Bitstream::appendPage() {
	++pageIndex;
	if (allocatedPages <= pageIndex) {
		// create new page
		book.push_back( new Word[PAGE_SIZE] );
		++allocatedPages;
	}
}

inline void Bitstream::appendWord() {
	if (++WordIndex >= PAGE_SIZE) {
		WordIndex = 0;
		appendPage();
		currentWord = &book[pageIndex][0];
	} else {
		currentWord++;
	}
}

inline void Bitstream::upCountBit() {
	if (++bitIndex >= BITS_PER_WORD) {
		bitIndex = 0;
		appendWord();
	}
}

inline void Bitstream::upCountBits( const unsigned short amount ) {
	bitIndex += amount;
	if (bitIndex >= BITS_PER_WORD) {
		bitIndex &= MASK_BIT_INDEX; // MODULO: BITS_PER_WORD
		appendWord();
	}
}

inline void Bitstream::downCountBits( const size_t amount ) {
	if (amount <= bitIndex) {
		bitIndex -= (unsigned short)amount;
		return; // pointer stays on the current Word
	}
	
	// combine all three indices to a single one and subtract the amount
	size_t singleIndex = numberOfBits();
	if (amount > singleIndex)
		singleIndex = 0;
	else
		singleIndex -= amount;
	
	// then split the index into three again
	pageIndex = (singleIndex >> SHIFT_PAGE);
	WordIndex = (singleIndex & MASK_WORD_INDEX) >> SHIFT_WORD;
	bitIndex  = (unsigned short)(singleIndex & MASK_BIT_INDEX);
	
	// move char to new position
	currentWord = &book[ pageIndex ][ WordIndex ];
}

//  ---------------------------------------------------------------
// |
// |  Read
// |
//  ---------------------------------------------------------------

bool Bitstream::read( const size_t idx ){
	Word* a = &book[ idx >> SHIFT_PAGE ][ (idx & MASK_WORD_INDEX) >> SHIFT_WORD ];
	unsigned short selectedBit = (unsigned short)(idx & MASK_BIT_INDEX);
	return (*a >> (MAX_INDEX_WORD - selectedBit)) & 1; // shift selected bit to lowest position
}

//  ---------------------------------------------------------------
// |
// |  Write
// |
//  ---------------------------------------------------------------

void Bitstream::add( const bool bit ) {
	*currentWord = (*currentWord << 1) | (int)bit;
	upCountBit();
}

void Bitstream::add( const Word input, unsigned short amount ) {
	// calculate if we have to split the char in two
	short overflow = bitIndex + amount - BITS_PER_WORD;
	
	if (overflow > 0) {
		// write current Word
		amount -= overflow;
		*currentWord = (*currentWord << amount) | ((input >> overflow) & BITS_MASK[amount]);
		upCountBits(amount);
		// and append to the next one
		*currentWord = (input & BITS_MASK[overflow]);
		upCountBits(overflow);
	} else {
		// everything fits into the current Word
		*currentWord = (*currentWord << amount) | (input & BITS_MASK[amount]);
		upCountBits(amount);
	}
}

unsigned short Bitstream::fillup( const bool fillWithOnes ) {
	if (bitIndex == 0)
		return 0; // no need to fill
	
	unsigned short missingBits = BITS_PER_WORD - bitIndex;
	
	*currentWord <<= missingBits; // for the last underfull byte append x bits
	if (fillWithOnes)
        *currentWord |= BITS_MASK[missingBits];
        
	upCountBits(missingBits);
	return missingBits;
}

void Bitstream::deleteBits( const size_t amount ) {
	unsigned short bitIndexBefore = bitIndex;
	downCountBits(amount);
	// we only care about the first char since the rest will be overwritten anyway
	if (amount <= bitIndexBefore)
		*currentWord >>= amount; // is actually the last char
	else
		*currentWord >>= (amount - bitIndexBefore) & MASK_BIT_INDEX; // subtract current bit index and all multiples of Word size
}

//  ---------------------------------------------------------------
// |
// |  Print
// |
//  ---------------------------------------------------------------

void Bitstream::print( const bool onlyCurrentPage ) {
	for (unsigned short page = 0; page <= pageIndex; page++) {
		if (page == pageIndex) // print the current page
			printPage(page, WordIndex + (bitIndex ? 1 : 0)); // omit last byte if no bit set (bitIndex==0)
		
		else if (onlyCurrentPage == false)
			printPage(page); // print all completely filled book
	}
}

// new line after 8 bytes (64bit)
const unsigned short breakByteAfter = 8 / WORD_SIZE;

void Bitstream::printPage( const size_t page, size_t truncate ) {
	printf("Page [%d]\n", (int)page);
	Word *a = &book[page][0];
	while (truncate--) {
		printWord(*(a++)); // move char pointer to the next char
		if (truncate % breakByteAfter == 0)
			printf("\n");
	}
	printf("\n");
}

void Bitstream::printWord( const Word &byte ) {
	unsigned short idx = BITS_PER_WORD;
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
	FILE* f = NULL;
#ifdef _WIN32
	fopen_s(&f, pathToFile, "wb");
#else
	f = fopen(pathToFile, "wb");
#endif
	
	char *byteRemap = new char[WORD_SIZE]; // needed to correct the byte order for int
	
	int bitsFilled = fillup(1); // complete the last byte
	
	size_t WordsOnLastPage = WordIndex;
	if (WordsOnLastPage > 0)
		WordsOnLastPage -= 1;// -1 = process last Word separately
	
	
	for (size_t page = 0; page <= pageIndex; page++) {
		size_t WordsOnPage = PAGE_SIZE;
		if (page == pageIndex)
			WordsOnPage = WordsOnLastPage; // save last page only partly
		
		Word *a = &book[page][0];
		
		// save values to binary file
		while (WordsOnPage--) {
			mapWordToChar(*(a++), byteRemap);
			fwrite(byteRemap, 1, WORD_SIZE, f); // move char pointer to the next char
		}
		
		// get last Word separately because it could contain 4 bytes even if we only need 1 byte
		if (page == pageIndex) {
			mapWordToChar(*a, byteRemap);
			fwrite(byteRemap, 1, WORD_SIZE - (bitsFilled / 8), f); // omit trailing 0xFF chars
		}
	}
	delete[] byteRemap;
	fclose(f);
	
	deleteBits(bitsFilled); // restore previous state
}

inline void Bitstream::mapWordToChar( const Word &in, char* &out ) {
	// this bitshift hack is possible because a char will copy only the 8 least significant bits
	switch (WORD_SIZE) {
		case 8: out[0]=(char)(in>>56); out[1]=(char)(in>>48); out[2]=(char)(in>>40); out[3]=(char)(in>>32);
			    out[4]=(char)(in>>24); out[5]=(char)(in>>16); out[6]=(char)(in>>8);  out[7]=(char)in; break;
		case 4: out[0]=(char)(in>>24); out[1]=(char)(in>>16); out[2]=(char)(in>>8);  out[3]=(char)in; break;
		case 2: out[0]=(char)(in>>8);  out[1]=(char)in; break;
		case 1: out[0]=(char)in; break;
	}
	// not readable, but compact and efficient
}

