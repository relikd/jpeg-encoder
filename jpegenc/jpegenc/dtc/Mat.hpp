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
#include <iostream>

struct Mat {
	
	int rows;
	int cols;
	bool isTransposed = false;
	
	
	Mat() {}
	Mat(int rows) : Mat(rows, rows) {}
	
	Mat(int rows, int cols) : rows(rows), cols(cols) {
		values = new float[rows * cols];
	}

	void transpose(bool transpose = true);
	
	void initiate(float* values, int rows, int cols);
	float get(int row, int col) const;
	float get(int index) const;
	void set(int row, int col, float value);
	void set(int index, float value);

	Mat operator+(const Mat &oMat) const;
	Mat operator*(const Mat &oMat) const;
	
	void print(int digit = 5);
    float* values;

private:
	int calculateIndex(int row, int col) const;
};


#endif /* Mat_hpp */
