//
//  Quantization.cpp
//  jpegenc
//
//  Created by Marcel Groß on 10.01.17.
//  Copyright © 2017 FHWS. All rights reserved.
//

#include "Quantization.hpp"
/*
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

*/

static const uint8_t luminaceQuantizationTable[64] = {
    1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1
};

static const uint8_t chrominanceQuantizationTable[64] = {
    1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1
};


void Quantization::run(float* &dctCoefficient, size_t picWidth, size_t picHeight, const uint8_t* &quantizationMatrix) {
	size_t matrixWidth = 8;
    size_t matrixLength = matrixWidth * matrixWidth;

    size_t horizontalBlocks = picWidth / matrixWidth;
    size_t verticalBlocks = picHeight / matrixWidth;
    
    size_t verticalOffset = 0;
    size_t horizontalOffset = 0;
    
    
    for (unsigned int verticalIndex = 0; verticalIndex < verticalBlocks; ++verticalIndex) {
        verticalOffset = verticalIndex * matrixWidth * picWidth;
        
        for (unsigned int horizontalIndex = 0; horizontalIndex < horizontalBlocks; ++horizontalIndex) {
            horizontalOffset = horizontalIndex * matrixWidth;
            
            for (int i = 0; i < matrixLength; ++i) {
                size_t innerBlockOffset = (i / matrixWidth) * (picWidth - matrixWidth);
                size_t totalOffset = verticalOffset + horizontalOffset + innerBlockOffset;

                dctCoefficient[i + totalOffset] = roundf(dctCoefficient[i + totalOffset] / quantizationMatrix[i]);
            }
        }
    }

}

const uint8_t* Quantization::getLuminanceQT() {
    return luminaceQuantizationTable;
}

const uint8_t* Quantization::getChrominanceQT() {
    return chrominanceQuantizationTable;
}
