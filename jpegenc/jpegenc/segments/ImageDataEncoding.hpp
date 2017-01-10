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

const int BLOCKDIMENSION = 8;
const int ZICK_ZACK_INDEXES[64] = {
	0, 1, 8, 16, 9, 2, 3, 10, 17, 24, 32, 25, 18, 11, 4, 5,
	12, 19, 26, 33, 40, 48, 41, 34, 27, 20, 13, 6, 7, 14, 21, 28,
	35, 42, 49, 56, 57, 50, 43, 36, 29, 22, 15, 23, 30, 37, 44, 51,
	58, 59, 52, 45, 38, 31, 39, 46, 53, 60, 61, 54, 47, 55, 62, 63 };

struct ImageDataEncoding {
	void encode(float* &input, float* &output, const unsigned int width, const unsigned int height);
};

#endif /* ImageDataEncoding_hpp */
