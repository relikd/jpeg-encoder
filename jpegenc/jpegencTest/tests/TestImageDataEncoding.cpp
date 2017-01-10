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


void initArray(float* data, const unsigned int length) {
	for (int i = 0; i < length; ++i) {
		data[i] = i;
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


TEST_CASE("Test zick zack encoding for image", "[zickzack]") {
	ImageDataEncoding encoding;
	unsigned int width = 16, height = 16;
	float* input = new float[width * height];
	float* output = new float[width * height];
	initArray(input, width * height);
	
	encoding.encode(input, output, width, height);
	printArray(input, width, height);
	printArray(output, width, height);
}
