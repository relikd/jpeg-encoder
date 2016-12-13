#include <exception>
#include "Mat.hpp"



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
		
		return get(col, row);
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
		
		set(col, row, value);
	}
	
	values[index] = value;
}

Mat Mat::operator+(const Mat &oMat) const {
	Mat mat(rows, cols);
	if (this->rows != oMat.rows || this->cols != oMat.cols) {
		fputs("Error can't add mats with different rows and cols!\n", stderr);
		throw std::exception();
	}
	
	int size = rows * cols;
	for (int i = 0; i < size; ++i) {
		mat.set(i, this->get(i) + oMat.get(i));
	}
	
	return mat;
	
	
}
Mat Mat::operator*(const Mat &oMat) const{
	return Mat(2);
}

int Mat::calculateIndex(int row, int col) const{
	return col + row * cols;
}
