//
//  TestImageDataEncoding.cpp
//  jpegenc
//
//  Created by Christian Braun on 10/01/17.
//  Copyright © 2017 FHWS. All rights reserved.
//

#include <stdio.h>
#include <iostream>
#include "catch.hpp"
#include "ImageDataEncoding.hpp"

float zickZackData[256] = {
	0, 1, 5, 6, 14, 15, 27, 28, 0, 1, 5, 6, 14, 15, 27, 28 ,
	2, 4, 7, 13, 16, 26, 29, 42, 2, 4, 7, 13, 16, 26, 29, 42,
	3, 8, 12, 17, 25, 30, 41, 43, 3, 8, 12, 17, 25, 30, 41, 43,
	9, 11, 18, 24, 31, 40, 44, 53, 9, 11, 18, 24, 31, 40, 44, 53,
	10, 19, 23, 32, 39, 45, 52, 54, 10, 19, 23, 32, 39, 45, 52, 54,
	20, 22, 33, 38, 46, 51, 55, 60, 20, 22, 33, 38, 46, 51, 55, 60,
	21, 34, 37, 47, 50, 56, 59, 61, 21, 34, 37, 47, 50, 56, 59, 61,
	35, 36, 48, 49, 57, 58, 62, 63, 35, 36, 48, 49, 57, 58, 62, 63,
	0, 1, 5, 6, 14, 15, 27, 28, 0, 1, 5, 6, 14, 15, 27, 28,
	2, 4, 7, 13, 16, 26, 29, 42, 2, 4, 7, 13, 16, 26, 29, 42,
	3, 8, 12, 17, 25, 30, 41, 43, 3, 8, 12, 17, 25, 30, 41, 43,
	9, 11, 18, 24, 31, 40, 44, 53, 9, 11, 18, 24, 31, 40, 44, 53,
	10, 19, 23, 32, 39, 45, 52, 54, 10, 19, 23, 32, 39, 45, 52, 54,
	20, 22, 33, 38, 46, 51, 55, 60, 20, 22, 33, 38, 46, 51, 55, 60,
	21, 34, 37, 47, 50, 56, 59, 61, 21, 34, 37, 47, 50, 56, 59, 61,
	35, 36, 48, 49, 57, 58, 62, 63, 35, 36, 48, 49, 57, 58, 62, 63 };

float lengthEncodingData[256] = {
	8, 12, 10, 2, 6, 10, 10, 12, 3, 3, 7, 4, 7, 11, 8, 3,
	1, 0, 7, 4, 2, 0, 2, 0, 4, 12, 5, 10, 6, 0, 7, 7,
	3, 2, 4, 3, 12, 0, 0, 11, 4, 2, 10, 10, 11, 4, 0, 3,
	0, 2, 2, 6, 6, 9, 0, 8, 7, 9, 4, 5, 0, 10, 11, 3,
	11, 9, 12, 8, 9, 0, 6, 11, 7, 3, 0, 12, 12, 11, 8, 0,
	10, 12, 11, 8, 10, 12, 10, 11, 4, 12, 5, 9, 0, 11, 0, 8,
	0, 11, 0, 7, 5, 0, 12, 0, 8, 0, 0, 12, 0, 0, 11, 9,
	6, 7, 9, 0, 0, 0, 9, 0, 0, 0, 6, 10, 8, 0, 10, 8,
	11, 8, 12, 9, 11, 0, 0, 11, 7, 0, 0, 9, 10, 0, 12, 0,
	0, 0, 0, 7, 7, 0, 0, 0, 0, 10, 0, 0, 8, 0, 0, 0,
	0, 8, 12, 0, 0, 0, 0, 9, 0, 0, 0, 0, 0, 0, 0, 11,
	0, 0, 11, 0, 0, 10, 0, 0, 0, 0, 12, 0, 0, 0, 12, 0,
	0, 10, 0, 11, 0, 11, 10, 0, 10, 0, 0, 0, 0, 0, 11, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 12, 0, 11, 0, 0, 0, 0, 0,
	0, 0, 12, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };


const unsigned int lengthEncodingBitRepsSize = 130;
const unsigned int lengthEncodingBitReps[lengthEncodingBitRepsSize] = {
	0xc, 0xa, 0x2, 0x6, 0xa, 0xa, 0xc, 0x3, 0x3, 0x7, 0x4, 0x7, 0xb, 0x8, 0x3, 0x1,
	0x17, 0x4, 0x2, 0x12, 0x14, 0xc, 0x5, 0xa, 0x6, 0x17, 0x7, 0x3, 0x2, 0x4, 0x3, 0xc,
	0x2b, 0x4, 0x2, 0xa, 0xa, 0xb, 0x4, 0x13, 0x12, 0x2, 0x6, 0x6, 0x9, 0x18, 0x7, 0x9,
	0x4, 0x5, 0x1a, 0xb, 0x3, 0x0, 0x9, 0xc, 0x8, 0x9, 0x16, 0xb, 0x7, 0x3, 0x1c, 0xc,
	0xb, 0x8, 0x1a, 0xc, 0xb, 0x8, 0xa, 0xc, 0xa, 0xb, 0x4, 0xc, 0x5, 0x9, 0x1b, 0x18,
	0x1b, 0x17, 0x5, 0x1c, 0x18, 0x2c, 0x2b, 0x9, 0x6, 0x7, 0x9, 0x39, 0x36, 0xa, 0x8, 0x1a,
	0x8, 0x0, 0x8, 0xc, 0x9, 0xb, 0x2b, 0x7, 0x29, 0xa, 0x1c, 0x47, 0x7, 0x4a, 0x28, 0x48,
	0xc, 0x49, 0x7b, 0x2b, 0x2a, 0x4c, 0x3c, 0x0, 0xa, 0x1b, 0x1b, 0xa, 0x1a, 0x5b, 0x9c, 0x1b, 0x7c, 0x0};


const unsigned int lengthDcEncodingResult = 4;
const Encoding dcEncodingResult[4] = {Encoding(8, 4), Encoding(2, 3), Encoding(8, 4), Encoding(4, 3)};
const unsigned int width = 16, height = 16;


void initArray(float* data, const unsigned int length) {
	for (int i = 0; i < length; ++i) {
		auto value = rand() % length;
		data[i] = value < i ? 0 : value / 10 ;
	}
}

void initArrayZickZack(float* data) {
	for (int i = 0; i < 64; ++i) {
		data[ZICK_ZACK_INDEXES[i]] = i;
	}
}

void printArray(float* data, const unsigned int width, const unsigned int height) {
	const unsigned int imagesize = width * height;
	
	for (int i = 0; i < imagesize; ++i) {
		std::cout << data[i] << " \t\t";
		if ((i + 1) % width == 0) {
			std::cout << std::endl;
		}
	}
	
	std::cout << std::endl;
}


bool isSortedCorrectly(float* data) {
	const int totalImageSize = width * height;
	for (int i = 0; i < totalImageSize; ++i) {
		if (data[i] != i % 64) {
			return false;
		}
	}
	
	return true;
}


TEST_CASE("Test image data encoding", "[imageDataEncoding]") {
	
	
	
	SECTION("Test zick zack encoding for image") {
		float* input = zickZackData;
		ImageDataEncoding encoding(input, width, height);
		encoding.init();
		
		REQUIRE(isSortedCorrectly(encoding.sortedData));
	}
	
	SECTION("Test calculate category") {
		auto result = ImageDataEncoding::calculateCategory(0);
		bool check0 = result.code == 0 && result.numberOfBits == 0;
		REQUIRE(check0);
		
		result = ImageDataEncoding::calculateCategory(32767);
		bool check32767 = result.code == 32767 && result.numberOfBits == 15;
		REQUIRE(check32767);
		
		result = ImageDataEncoding::calculateCategory(-32766);
		bool checkNegativ32766 = result.code == 1 && result.numberOfBits == 15;
		REQUIRE(checkNegativ32766);
	}
	
	SECTION("Test bit combining") {
		uint8_t byte = ImageDataEncoding::toSingleByte(0, 0);
		REQUIRE(byte == 0);
		
		byte = ImageDataEncoding::toSingleByte(31, 2);
		REQUIRE(byte == 242);
	}
	
	SECTION("Test run length encoding") {
		std::vector<uint8_t> byteReps;
		std::vector<Encoding> encodings;
		float* input = lengthEncodingData;
		ImageDataEncoding encoding(input, width, height);
		
		// Don't do that! Only for testing purposes
		encoding.sortedData = input;
		int length = encoding.runLengthEncoding(byteReps, encodings);
		
		REQUIRE(length == lengthEncodingBitRepsSize);
		REQUIRE(byteReps.size() == lengthEncodingBitRepsSize);
		REQUIRE(encodings.size() == lengthEncodingBitRepsSize);
		
		for(int i = 0; i < length; ++i) {
			REQUIRE(byteReps[i] == lengthEncodingBitReps[i]);
		}
	}
	
	SECTION("Test difference encoding") {
		float* input = lengthEncodingData;
		ImageDataEncoding encoding(input, width, height);
		encoding.init();
		auto dcEncodings = encoding.differenceEncoding();
		
		int length = encoding.verticalBlocks * encoding.horizontalBlocks;
		
		REQUIRE(length == lengthDcEncodingResult);
		REQUIRE(dcEncodings.size() == lengthDcEncodingResult);
		
		for (int i = 0; i < length; ++i) {
			bool validEncoding =
			dcEncodings[i].numberOfBits == dcEncodingResult[i].numberOfBits &&
			dcEncodings[i].code == dcEncodingResult[i].code;
			
			REQUIRE(validEncoding);
		}
	}
	
	SECTION("Test AC Huffman encoding table") {
		std::vector<uint8_t> byteReps;
		std::vector<Encoding> encodings;
		float* input = lengthEncodingData;
		
		ImageDataEncoding encoding(input, width, height);
		encoding.init();
		
		auto encodingTable = encoding.generateACEncodingTable(byteReps, encodings);
		auto root = Huffman::treeFromEncodingTable(encodingTable);
		
	}
	
	SECTION("Test DC Huffman encoding table") {
		std::vector<Encoding> encodings;
		float* input = lengthEncodingData;
		
		ImageDataEncoding encoding(input, width, height);
		encoding.init();
		
		auto encodingTable = encoding.generateDCEncodingTable(encodings);
		auto root = Huffman::treeFromEncodingTable(encodingTable);
		
	}
}
