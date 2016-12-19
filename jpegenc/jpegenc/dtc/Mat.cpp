#include <exception>
#include <iostream>
#include "Mat.hpp"
#include <math.h>


float round(float value)
{
	return roundf(value * 100000) / 100000.0F;
}


void Mat::transpose(bool transpose) {
	isTransposed = transpose;
	int temp = rows;
	rows = cols;
	cols = temp;
}

/**
 * Initiate will override the current values of the mat.
 */
void Mat::initiate(float* values, int rows, int cols) {
	this->rows = rows;
	this->cols = cols;
	this->values = values;
}

float Mat::get(int row, int col) const {
	if (isTransposed) {
		return values[calculateIndex(col, row)];
	} else {
		return values[calculateIndex(row, col)];
	}
}

float Mat::get(int index) const{
	if (isTransposed) {
		int row = index / cols;
		int col = index % cols;
		
		return get(row, col);
	}
	
	return values[index];
}

void Mat::set(int row, int col, float value) {
	values[calculateIndex(row, col)] = value;
}

void Mat::set(int index, float value) {
	if (isTransposed) {
		int row = index / cols;
		int col = index % cols;
		
		set(row, col, value);
	}
	
	values[index] = value;
}

Mat Mat::operator+(const Mat &oMat) const {
	if (this->rows != oMat.rows || this->cols != oMat.cols) {
		fputs("Error can't add mats with different rows and cols!\n", stderr);
		throw std::exception();
	}
	
	Mat mat(rows, cols);
	int size = rows * cols;
	for (int i = 0; i < size; ++i) {
		mat.set(i, this->get(i) + oMat.get(i));
	}
	
	return mat;
	
	
}
Mat Mat::operator*(const Mat &oMat) const{
	if (this->cols != oMat.rows) {
		fputs("Error can't multiply mats with different cols to rows!\n", stderr);
		throw std::exception();
	}
	
	Mat mat(this->rows, oMat.cols);
	for(int oMatColumn = 0; oMatColumn < oMat.cols; ++oMatColumn) {
		for(int currentRow = 0; currentRow < rows; ++currentRow) {
			float result = 0;
			for(int currentCol = 0; currentCol < cols; ++currentCol) {
				result += this->get(currentRow, currentCol) * oMat.get(currentCol, oMatColumn);
			}
			mat.set(currentRow, oMatColumn, result);
		}
	}

	return mat;
}

int Mat::calculateIndex(int row, int col) const{
	if (isTransposed) {
		return col + row * rows;
	} else {
		return col + row * cols;
	}
	
}

void Mat::print() {
	for (int i = 0 ; i < rows; ++i) {
		for (int k = 0; k < cols; ++k) {
			std::cout << round(get(i, k)) << "\t";
		}
		std::cout << std::endl;
	}
}
