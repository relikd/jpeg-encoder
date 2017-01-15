//
//  Quantization.cpp
//  jpegenc
//
//  Created by Marcel Groß on 10.01.17.
//  Copyright © 2017 FHWS. All rights reserved.
//

#include "Quantization.hpp"


void Quantization::run(float* &dctCoefficient, float* &quantizationMatrix){
	size_t blocksPerLine = 8;
	size_t numberOfBlocks = 64;
	size_t blockWidth = 8;
	size_t imageWidth = blocksPerLine * blockWidth;
	size_t matrixLength = blockWidth * blockWidth;
	
	for (unsigned int i = 0; i < numberOfBlocks; i++) {
		for (unsigned int j = 0; j < matrixLength; j++) {
			dctCoefficient[i * imageWidth + j] = dctCoefficient[i * imageWidth + j] / quantizationMatrix[(i % blockWidth) * blockWidth + j % blockWidth];
		}
	}
}
