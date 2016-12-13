//
//  Mat.hpp
//  jpegenc
//
//  Created by Marcel Groß on 13.12.16.
//  Copyright © 2016 FHWS. All rights reserved.
//

#ifndef Mat_hpp
#define Mat_hpp

#include <stdio.h>
#include "BitMath.hpp"

struct Mat {
	
	const int rows;
	const int cols;
	bool isTransposed = false;
	
	
	Mat(int rows) : Mat(rows, rows) {}
	
	Mat(int rows, int cols) : rows(rows), cols(cols), bitsForRows(BitMath::log2(rows)), bitsForCols(BitMath::log2(cols)) {
		values = new float[rows * cols];
	}
	
	void transpose(bool transpose = true) {
		isTransposed = transpose;
	}
	
	float get(int row, int col) const;
	float get(int index) const;
	void set(int row, int col, float value);
	void set(int index, float value);

	Mat operator+(const Mat &oMat) const;
	Mat operator*(const Mat &oMat) const;

private:
	const int bitsForRows;
	const int bitsForCols;
	float* values;
	int calculateIndex(int row, int col) const;
};


#endif /* Mat_hpp */
