#include "ImageDataEncoding.hpp"
#include <iostream>

void ImageDataEncoding::encode(float* &input, float* &output, const unsigned int width, const unsigned int height) {
	const unsigned int horizontalBlocks = width / BLOCKDIMENSION;
	const unsigned int verticalBlocks = height / BLOCKDIMENSION;
	const unsigned int totalBlockSize = BLOCKDIMENSION * BLOCKDIMENSION;
	
	unsigned int verticalOffset = 0;
	unsigned int horizontalOffset = 0;
	unsigned int outputIndex = 0;
	

	for (int verticalIndex = 0; verticalIndex < verticalBlocks; ++verticalIndex) {
		verticalOffset = verticalIndex * BLOCKDIMENSION * width;
		
		for (int horizontalIndex = 0; horizontalIndex < horizontalBlocks; ++horizontalIndex) {
			horizontalOffset = horizontalIndex * BLOCKDIMENSION;
			
			for (int i = 0; i < totalBlockSize; ++i) {
				unsigned int innerBlockOffset = (ZICK_ZACK_INDEXES[i] / BLOCKDIMENSION) * (width - BLOCKDIMENSION);
				unsigned int totalOffset = verticalOffset + horizontalOffset + innerBlockOffset;
				output[outputIndex++] = input[ZICK_ZACK_INDEXES[i] + totalOffset];
			}
		}
	}
}
