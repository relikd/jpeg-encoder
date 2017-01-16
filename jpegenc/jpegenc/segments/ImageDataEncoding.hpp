//
//  ImageDataEncoding.hpp
//  jpegenc
//
//  Created by Christian Braun on 10/01/17.
//  Copyright © 2017 FHWS. All rights reserved.
//

#ifndef ImageDataEncoding_hpp
#define ImageDataEncoding_hpp

#include <stdio.h>
#include <vector>
#include "../huffman/Encoding.hpp"
#include "../huffman/Huffman.hpp"

const int BLOCKDIMENSION = 8;
const int TOTAL_BLOCK_SIZE = BLOCKDIMENSION * BLOCKDIMENSION;
const int ZICK_ZACK_INDEXES[64] = {
	0, 1, 8, 16, 9, 2, 3, 10, 17, 24, 32, 25, 18, 11, 4, 5,
	12, 19, 26, 33, 40, 48, 41, 34, 27, 20, 13, 6, 7, 14, 21, 28,
	35, 42, 49, 56, 57, 50, 43, 36, 29, 22, 15, 23, 30, 37, 44, 51,
	58, 59, 52, 45, 38, 31, 39, 46, 53, 60, 61, 54, 47, 55, 62, 63 };

struct ImageDataEncoding {
	const unsigned int width, height;
	const unsigned int horizontalBlocks, verticalBlocks;
	float* data;
	float* sortedData;
	
	ImageDataEncoding(float* data, const unsigned int width, const unsigned int height)
	:
	width(width),
	height(height),
	horizontalBlocks(width / BLOCKDIMENSION),
	verticalBlocks(height / BLOCKDIMENSION),
	data(data),
	sortedData(new float[width * height]) {};
	
	~ImageDataEncoding() {
//		delete []sortedData;
	}
	
	static Encoding calculateCategory(int input);
	static uint8_t toSingleByte(unsigned long one, unsigned long two);

	
	void init();
	EncodingTable generateACEncodingTable(std::vector<uint8_t> &byteReps, std::vector<Encoding> &encodings);
	EncodingTable generateDCEncodingTable(std::vector<Encoding> &encodings);
	void sortZickZack();
	std::vector<Encoding> differenceEncoding();
	unsigned int runLengthEncoding(std::vector<uint8_t> &byteRepresentations, std::vector<Encoding> &encodings);
	unsigned int runLengthEncodingSingleBlock(std::vector<uint8_t> &byteRepresentations, std::vector<Encoding> &encodings, unsigned int offset, unsigned int encodingIndex);
	
	void addEndOfBlock(std::vector<uint8_t> &byteRepresentations, std::vector<Encoding> &encodings, unsigned int index);
};

#endif /* ImageDataEncoding_hpp */
