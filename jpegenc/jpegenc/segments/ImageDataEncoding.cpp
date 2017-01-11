#include "ImageDataEncoding.hpp"
#include <iostream>
#include "../helper/BitMath.hpp"
#include "../bitstream/Bitstream.hpp"

void ImageDataEncoding::encode(float* &input, float* &output) {
	const unsigned int horizontalBlocks = width / BLOCKDIMENSION;
	const unsigned int verticalBlocks = height / BLOCKDIMENSION;

	unsigned int verticalOffset = 0;
	unsigned int horizontalOffset = 0;
	unsigned int outputIndex = 0;
	

	for (int verticalIndex = 0; verticalIndex < verticalBlocks; ++verticalIndex) {
		verticalOffset = verticalIndex * BLOCKDIMENSION * width;
		
		for (int horizontalIndex = 0; horizontalIndex < horizontalBlocks; ++horizontalIndex) {
			horizontalOffset = horizontalIndex * BLOCKDIMENSION;
			
			for (int i = 0; i < TOTAL_BLOCK_SIZE; ++i) {
				unsigned int innerBlockOffset = (ZICK_ZACK_INDEXES[i] / BLOCKDIMENSION) * (width - BLOCKDIMENSION);
				unsigned int totalOffset = verticalOffset + horizontalOffset + innerBlockOffset;
				output[outputIndex++] = input[ZICK_ZACK_INDEXES[i] + totalOffset];
			}
		}
	}
}

Encoding ImageDataEncoding::calculateCategory(int input) {
	if (input == 0) {
		return Encoding(0 , 0);
	}
	
	int start;
	int bits;
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

/**
* @param input represents the cosinus transformed and zick zack sorted image.
* @param byteRepresentations will be filled with the bytes of the length encoding.
* @param encodings will be filled with the encodings of the length encoding.
* byteRepresentations[0] and encodings[0] and so on fit together.
*/
unsigned int ImageDataEncoding::runLengthEncoding(float* &input, uint8_t* &byteRepresentations, Encoding* &encodings) {
	int encodingIndex = 0;
	
	for (int i = 1; i < width * height; i += 64) {
		encodingIndex = runLengthEncodingSingleBlock(input, byteRepresentations, encodings, i, encodingIndex);
	}
	
	return encodingIndex;
}

unsigned int ImageDataEncoding::runLengthEncodingSingleBlock(float* &input, uint8_t* &byteRepresentations,
													 Encoding* &encodings, unsigned int offset, unsigned int encodingIndex) {
	const int maxZerosInARow = 15;
	int zerosInARow = 0;
	int lastIndexEOB = INT_MAX;
	
	for (int i = offset; i < offset + TOTAL_BLOCK_SIZE - 1; ++i) {
		if (input[i] == 0 && zerosInARow != maxZerosInARow) {
			++zerosInARow;
			continue;
		}
		
		if (input[i] == 0 && lastIndexEOB > encodingIndex) {
			lastIndexEOB = encodingIndex;
		} else {
			lastIndexEOB = INT_MAX;
		}
		
		encodings[encodingIndex] = calculateCategory(input[i]);
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

/**
* toSingleByte takes the least 4 bits of one and two and combines them
* to 8 bytes with one in the front.
*/
uint8_t ImageDataEncoding::toSingleByte(char one, char two) {
	return (one << 4)| two;
}

void ImageDataEncoding::addEndOfBlock(uint8_t* &byteRepresentations, Encoding* &encodings, unsigned int index) {
	encodings[index] = calculateCategory(0);
	byteRepresentations[index] = toSingleByte(0, encodings[index].code);
}








