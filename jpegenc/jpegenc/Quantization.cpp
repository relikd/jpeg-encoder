//
//  Quantization.cpp
//  jpegenc
//
//  Created by Marcel Groß on 10.01.17.
//  Copyright © 2017 FHWS. All rights reserved.
//

#include "Quantization.hpp"

void Quantization::run(float* &dctCoefficient, size_t picWidth, size_t picHeigth, float* &quantizationMatrix) {
	size_t matrixWidth = 8;
	size_t numberOfBlocks = (picWidth * picHeigth) / (matrixWidth * matrixWidth);
	size_t matrixLength = matrixWidth * matrixWidth;
	
	for (unsigned int i = 0; i < numberOfBlocks; i++) {
		for (unsigned int j = 0; j < matrixLength; j++) {
			dctCoefficient[i * picWidth + j] = dctCoefficient[i * picWidth + j] / quantizationMatrix[(i % matrixWidth) * matrixWidth + j % matrixWidth];
		}
	}
}
