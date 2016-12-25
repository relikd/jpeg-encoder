#ifndef directDCT_hpp
#define directDCT_hpp

#include <stdio.h>
#include "Mat.hpp"

struct DCT {
	static Mat transform(Mat input);
	static void transform(float* &input, float* &output, const size_t width, const size_t height);
	static Mat transform2(Mat input);
	static void transform2(float* &input, const size_t width, const size_t height);
	static Mat inverse(Mat input);
	static void inverse(float* &input, float* &output);
private:
	static Mat generateA(int dimension);
};


#endif /* directDCT_hpp */
