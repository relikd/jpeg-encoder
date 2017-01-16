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
* @param byteReps will be filled with the byte representations of the encodings in the order they appear.
* @param encodings will be filled with the encodings for every dct value in the order they appear.
*/
EncodingTable ImageDataEncoding::generateACEncodingTable(std::vector<uint8_t> &byteReps, std::vector<Encoding> &encodings) {
	unsigned int length = runLengthEncoding(byteReps, encodings);
	Huffman huffman;
	
	for (unsigned int i = 0; i < length; ++i) {
		huffman.addSymbol(byteReps[i]);
	}
	huffman.generateNodeList();
	
	return huffman.canonicalEncoding(16);
}

/**
* @param encodings will be filled with the encodings for every dct value in the order they appear.
*/
EncodingTable ImageDataEncoding::generateDCEncodingTable(std::vector<Encoding> &encodings) {
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

std::vector<Encoding> ImageDataEncoding::differenceEncoding() {
	std::vector<Encoding> encodings;
	const unsigned int rowOffset = width * BLOCKDIMENSION;
	unsigned int blockInRow = 0;
	unsigned int imageSize = width * height;
	
	
	encodings.push_back(calculateCategory(sortedData[0]));
	
	for (int dcIndex = TOTAL_BLOCK_SIZE; dcIndex < imageSize; dcIndex += TOTAL_BLOCK_SIZE, ++blockInRow) {
		int neighborBlockOffset = TOTAL_BLOCK_SIZE;
		
		if (blockInRow == horizontalBlocks) {
			blockInRow = 0;
			neighborBlockOffset = rowOffset;
		}
		
		encodings.push_back(calculateCategory(sortedData[dcIndex] - sortedData[dcIndex - neighborBlockOffset]));
	}
	
	return encodings;
}

/**
* @param byteRepresentations will be filled with the bytes of the length encoding.
* @param encodings will be filled with the encodings of the length encoding.
* byteRepresentations[0] and encodings[0] and so on fit together.
*/
unsigned int ImageDataEncoding::runLengthEncoding(std::vector<uint8_t> &byteRepresentations,std::vector<Encoding> &encodings) {
	int encodingIndex = 0;
	
	for (unsigned int i = 1; i < width * height; i += 64) {
		encodingIndex = runLengthEncodingSingleBlock(byteRepresentations, encodings, i, encodingIndex);
	}
	
	return encodingIndex;
}

unsigned int ImageDataEncoding::runLengthEncodingSingleBlock(std::vector<uint8_t> &byteRepresentations,
													std::vector<Encoding> &encodings, unsigned int offset, unsigned int encodingIndex) {
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
		
		encodings.push_back(calculateCategory((int)sortedData[i]));
		byteRepresentations.push_back(toSingleByte(zerosInARow, encodings[encodingIndex].numberOfBits));
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


void ImageDataEncoding::addEndOfBlock(std::vector<uint8_t> &byteRepresentations, std::vector<Encoding> &encodings, unsigned int index) {
	encodings.erase(encodings.begin() + index, encodings.end());
	encodings.push_back(calculateCategory(0));
	
	byteRepresentations.erase(byteRepresentations.begin() + index, byteRepresentations.end());
	byteRepresentations.push_back(toSingleByte(0, encodings[index].code));
}
