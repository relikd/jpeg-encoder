#include "ImageDataEncoding.hpp"
#include <iostream>
#include "../helper/BitMath.hpp"
#include "../bitstream/Bitstream.hpp"


/**
 * toSingleByte takes the least 4 bits of one and two and combines them
 * to 8 bits with one in the front.
 */
uint8_t ImageDataEncoding::toSingleByte(unsigned long one, unsigned long two) {
	return (uint8_t)((one << 4) | two);
}


Encoding ImageDataEncoding::calculateCategory(int input) {
	if (input == 0) {
		return Encoding(0 , 0);
	}
	
	int start;
	unsigned short bits;
	if (input < 0) {
		int temp = input * -1;
		bits = BitMath::log2(temp);
		start = (int)BITS_MASK[bits];
	} else {
		start = 0;
		bits = BitMath::log2(input);
	}
	
	return Encoding(start + input, bits);
}


void ImageDataEncoding::init() {
	sortZickZack();
}

/**
* @param byteReps has to be handed in as an empty array with the size of the total image
* and will be filled with the byte representations of the encodings in the order they appear.
* @param encodings has to be handed in as an empty array with the size of the total image
* and will be filled with the encodings for every dct value in the order they appear.
*/
EncodingTable ImageDataEncoding::generateACEncodingTable(uint8_t* byteReps, Encoding* encodings) {
	unsigned int length = runLengthEncoding(byteReps, encodings);
	Huffman huffman;
	
	for (unsigned int i = 0; i < length; ++i) {
		huffman.addSymbol(byteReps[i]);
	}
	huffman.generateNodeList();
	
	return huffman.canonicalEncoding(16);
}

/**
* @param encodings has to be handed in as an empty array with the size of the total image
* and will be filled with the encodings for every dct value in the order they appear.
*/
EncodingTable ImageDataEncoding::generateDCEncodingTable(Encoding* encodings) {
	encodings = differenceEncoding();
	Huffman huffman;
	
	for (unsigned int i = 0; i < verticalBlocks * horizontalBlocks; ++i) {
		huffman.addSymbol(encodings[i].numberOfBits);
	}
	huffman.generateNodeList();
	
	return huffman.canonicalEncoding(16);
}

void ImageDataEncoding::sortZickZack() {
	const unsigned int horizontalBlocks = width / BLOCKDIMENSION;
	const unsigned int verticalBlocks = height / BLOCKDIMENSION;

	unsigned int verticalOffset = 0;
	unsigned int horizontalOffset = 0;
	unsigned int outputIndex = 0;
	

	for (unsigned int verticalIndex = 0; verticalIndex < verticalBlocks; ++verticalIndex) {
		verticalOffset = verticalIndex * BLOCKDIMENSION * width;
		
		for (unsigned int horizontalIndex = 0; horizontalIndex < horizontalBlocks; ++horizontalIndex) {
			horizontalOffset = horizontalIndex * BLOCKDIMENSION;
			
			for (int i = 0; i < TOTAL_BLOCK_SIZE; ++i) {
				unsigned int innerBlockOffset = (ZICK_ZACK_INDEXES[i] / BLOCKDIMENSION) * (width - BLOCKDIMENSION);
				unsigned int totalOffset = verticalOffset + horizontalOffset + innerBlockOffset;
				sortedData[outputIndex++] = data[ZICK_ZACK_INDEXES[i] + totalOffset];
			}
		}
	}
}

Encoding* ImageDataEncoding::differenceEncoding() {
	Encoding* encodings = new Encoding[verticalBlocks * horizontalBlocks];
	
	unsigned int blockIndex = 0;
	// Handling first Block
	encodings[blockIndex++] = calculateCategory((int)sortedData[0]);
	
	// Handling the first line of blocks separately
	for (unsigned int firstLineBlock = 64 ; firstLineBlock < horizontalBlocks * TOTAL_BLOCK_SIZE; firstLineBlock += TOTAL_BLOCK_SIZE) {
		encodings[blockIndex++] = calculateCategory((int)sortedData[firstLineBlock] - TOTAL_BLOCK_SIZE);
	}
	
	// Handling the rest
	for (unsigned int verticalBlockIndex = 1; verticalBlockIndex < verticalBlocks; ++verticalBlockIndex) {
		unsigned int verticalOffset = verticalBlockIndex * width * BLOCKDIMENSION;
		
		for (unsigned int horizontalBlockIndex = 0; horizontalBlockIndex < horizontalBlocks; ++horizontalBlockIndex) {
			unsigned int horizontalOffset = horizontalBlockIndex * TOTAL_BLOCK_SIZE;
			unsigned int dcIndex =  verticalOffset + horizontalOffset;
			float leftValue = 0;
			float upperValue = sortedData[dcIndex - BLOCKDIMENSION * width];
			
			if (horizontalBlockIndex != 0) {
				leftValue = sortedData[dcIndex - TOTAL_BLOCK_SIZE];
			}
			
			encodings[blockIndex++] = calculateCategory((int)(sortedData[dcIndex] - leftValue - upperValue));
		}
	}
	
	return encodings;
}

/**
* @param byteRepresentations will be filled with the bytes of the length encoding.
* @param encodings will be filled with the encodings of the length encoding.
* byteRepresentations[0] and encodings[0] and so on fit together.
*/
unsigned int ImageDataEncoding::runLengthEncoding(uint8_t* byteRepresentations, Encoding* encodings) {
	int encodingIndex = 0;
	
	for (unsigned int i = 1; i < width * height; i += 64) {
		encodingIndex = runLengthEncodingSingleBlock(byteRepresentations, encodings, i, encodingIndex);
	}
	
	return encodingIndex;
}

unsigned int ImageDataEncoding::runLengthEncodingSingleBlock(uint8_t* byteRepresentations,
													 Encoding* encodings, unsigned int offset, unsigned int encodingIndex) {
	const unsigned int maxZerosInARow = 15;
	unsigned int zerosInARow = 0;
	unsigned int lastIndexEOB = INT_MAX;
	
	for (unsigned int i = offset; i < offset + TOTAL_BLOCK_SIZE - 1; ++i) {
		if (sortedData[i] == 0 && zerosInARow != maxZerosInARow) {
			++zerosInARow;
			continue;
		}
		
		if (sortedData[i] == 0 && lastIndexEOB > encodingIndex) {
			lastIndexEOB = encodingIndex;
		} else {
			lastIndexEOB = INT_MAX;
		}
		
		encodings[encodingIndex] = calculateCategory((int)sortedData[i]);
		byteRepresentations[encodingIndex] = toSingleByte(zerosInARow, encodings[encodingIndex].code);
		++encodingIndex;
		
		zerosInARow = 0;
	}
	
	if (lastIndexEOB < INT_MAX) {
		addEndOfBlock(byteRepresentations, encodings, lastIndexEOB);
		encodingIndex = lastIndexEOB + 1;
		
	} else {
		addEndOfBlock(byteRepresentations, encodings, encodingIndex);
		++encodingIndex;
	}
	
	return encodingIndex;
}


void ImageDataEncoding::addEndOfBlock(uint8_t* byteRepresentations, Encoding* encodings, unsigned int index) {
	encodings[index] = calculateCategory(0);
	byteRepresentations[index] = toSingleByte(0, encodings[index].code);
}


float ImageDataEncoding::getValueOnIndex(long index) {
	if (index < 0 || index > (long)(width * height)) {
		return 0;
	} else {
		return sortedData[index];
	}
}






