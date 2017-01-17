//
//  Quantization.cpp
//  jpegenc
//
//  Created by Marcel Groß on 10.01.17.
//  Copyright © 2017 FHWS. All rights reserved.
//

#include "Quantization.hpp"

static const uint8_t luminaceQuantizationTable[64] = {
    16, 11, 10, 16, 24, 40, 51, 61,
    12, 12, 14, 19, 26, 58, 60, 55,
    14, 13, 16, 24, 40, 57, 69, 56,
    14, 17, 22, 29, 51, 87, 80, 62,
    18, 22, 37, 56, 68, 109, 103, 77,
    24, 35, 55, 64, 81, 104, 113, 92,
    49, 64, 78, 87, 103, 121, 120, 101,
    72, 92, 95, 98, 112, 100, 103, 99
};

static const uint8_t chrominanceQuantizationTable[64] = {
    17, 18, 24, 47, 99, 99, 99, 99,
    18, 21, 26, 66, 99, 99, 99, 99,
    24, 26, 56, 99, 99, 99, 99, 99,
    47, 66, 99, 99, 99, 99, 99, 99,
    99, 99, 99, 99, 99, 99, 99, 99,
    99, 99, 99, 99, 99, 99, 99, 99,
    99, 99, 99, 99, 99, 99, 99, 99,
    99, 99, 99, 99, 99, 99, 99, 99
};


void Quantization::run(float* &dctCoefficient, size_t picWidth, size_t picHeigth, const uint8_t* &quantizationMatrix) {
	size_t matrixWidth = 8;
	size_t numberOfBlocks = (picWidth * picHeigth) / (matrixWidth * matrixWidth);
	size_t matrixLength = matrixWidth * matrixWidth;
	
	for (unsigned int i = 0; i < numberOfBlocks; i++) {
		for (unsigned int j = 0; j < matrixLength; j++) {
			dctCoefficient[i * picWidth + j] = roundf(dctCoefficient[i * picWidth + j] / quantizationMatrix[(i % matrixWidth) * matrixWidth + j % matrixWidth]);
		}
	}
}

const uint8_t* Quantization::getLuminanceQT() {
    return luminaceQuantizationTable;
}

const uint8_t* Quantization::getChrominanceQT() {
    return chrominanceQuantizationTable;
}
