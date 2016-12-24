#ifndef directDCT_hpp
#define directDCT_hpp

#include <stdio.h>
#include "Mat.hpp"

struct DCT {
	static Mat transform(Mat input);
	static void transform(float* &input, float* &output, size_t width, size_t height);
	static Mat transform2(Mat input);
	static void transform2(float* &input);
	static Mat inverse(Mat input);
private:
	static Mat generateA(int dimension);
};


#endif /* directDCT_hpp */
