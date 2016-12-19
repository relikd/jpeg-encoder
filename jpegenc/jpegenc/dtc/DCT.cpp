//
//  directDCT.cpp
//  jpegenc
//
//  Created by Marcel Groß on 13.12.16.
//  Copyright © 2016 FHWS. All rights reserved.
//

#define _USE_MATH_DEFINES

#include "DCT.hpp"
#include <math.h>

Mat DCT::transform(Mat input) {
	// As mat has to be quadratic we can just work with the rows
	Mat newMat(input.rows);
	for (int i = 0; i < input.rows; ++i) {
		for (int j = 0; j < input.rows; ++j) {
		
			float outer = 2/input.rows * getC(i) * getC(j);
			float inner = 0;
	
			for (int x = 0; x < input.rows; ++x) {
				for (int y = 0; y < input.rows; ++y) {
					float firstCos = cos(((2*x + 1) * i * M_PI) / (2.0 * input.rows));
					float secondCos = cos(((2*y + 1) * j * M_PI) / (2.0 * input.rows));
					inner += input.get(x, y) * firstCos * secondCos;
				
				}
			}
			newMat.set(i , j , outer * inner);
		}
	}
	
	return newMat;
}

Mat DCT::transform2DDCT(Mat input) {
	Mat a = generateA(input.rows);
	Mat temp = a * input;
	a.transpose();
	
	return temp  * a;
}

Mat DCT::inverse(Mat input) {
	// As mat has to be quadratic we can just work with the rows
	Mat newMat(input.rows);
	
	for (int x = 0; x < input.rows; ++x) {
		for (int y = 0; y < input.rows; ++y) {
			
			float inner = 0;
			
			for (int i = 0; i < input.rows; ++i) {
				for (int j = 0; j < input.rows; ++j) {
					float präfix = 2.0 /input.rows * getC(i) * getC(j) * input.get(i, j);
					float firstCos = cos(((2 * x + 1) * i * M_PI) / (2.0 * input.rows));
					float secondCos = cos(((2 * y + 1) * j * M_PI) / (2.0 * input.rows));
					inner += präfix * firstCos * secondCos;
					
				}
			}
			newMat.set(x, y, inner);
		}
	}
	
	return newMat;
}

float DCT::getC(int i) {
	if (i == 0) {
		return 1.0/sqrt(2);
	}
	return 1;
}

Mat DCT::generateA(int dimension) {
	Mat mat(dimension);
	
	for (int k = 0; k < dimension; ++k) {
		for (int n = 0; n < dimension; ++n) {
			float value = getC(k) * sqrt(2.0 / dimension) * cos((2 * n + 1) * ((k * M_PI) / (2 * dimension)));
			mat.set(k, n, value);
		}
	}
	
	return mat;
}
