//
//  IDCT.cpp
//  jpegenc
//
//  Created by Marcel Groß on 13.12.16.
//  Copyright © 2016 FHWS. All rights reserved.
//
#define _USE_MATH_DEFINES

#include <cmath>

#include "IDCT.hpp"
#include <math.h>

float IDCT::getC(int i) {
	if (i == 0) {
		return 1/sqrt(2);
	}
	
	return 1;
}

Mat IDCT::transform(Mat input){
	// As mat has to be quadratic we can just work with the rows
	Mat newMat(input.rows);
	
	for (int x = 0; x < input.rows; ++x) {
		for (int y = 0; y < input.rows; ++y) {
			
			float inner = 0;
			
			for (int i = 0; i < input.rows; ++i) {
				for (int j = 0; j < input.rows; ++j) {
					float präfix = 2/input.rows * getC(i) * getC(j) * input.get(i, j);
					float firstCos = cos(((2*x + 1) * i * M_PI) / (2 * input.rows));
					float secondCos = cos(((2*y + 1) * j * M_PI) / (2 * input.rows));
					inner += präfix * firstCos * secondCos;
					
				}
			}
			newMat.set(x, y, inner);
		}
	}
	
	return newMat;
}

