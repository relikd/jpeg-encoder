//
//  directDCT.cpp
//  jpegenc
//
//  Created by Marcel Groß on 13.12.16.
//  Copyright © 2016 FHWS. All rights reserved.
//

#define _USE_MATH_DEFINES

#include <cmath>

#include "directDCT.hpp"
#include <math.h>

Mat DirectDCT::transform(Mat input) {
	Mat newMat(input.N);
	for (int i = 0; i < input.N; ++i) {
		for (int j = 0; j < input.N; ++j) {
		
			float outer = 2/input.N * getC(i) * getC(j);
			float inner = 0;
	
			for (int x = 0; x < input.N; ++x) {
				for (int y = 0; y < input.N; ++y) {
					float firstCos = cos(((2*x + 1) * i * M_PI) / (2 * input.N));
					float secondCos = cos(((2*y + 1) * j * M_PI) / (2 * input.N));
					inner += input.mat[x][y] * firstCos * secondCos;
				
				}
			}
			newMat.mat[i][j] = outer * inner;
		}
	}
	
	return newMat;
}

float DirectDCT::getC(int i) {
	if (i == 0) {
		return 1/sqrt(2);
	}
	return 1;
}
