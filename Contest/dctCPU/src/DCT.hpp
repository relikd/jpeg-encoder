#ifndef directDCT_hpp
#define directDCT_hpp

#include <stdio.h>

struct DCT {
	static void transform(float* input, float* output, const size_t width, const size_t height);
	static void transform2(float* input, const size_t width, const size_t height);
	static void inverse(float* input, float* output, const size_t width, const size_t height);
	
	static void transformMT(float* input, float* output, const size_t width, const size_t height);
	static void transform2MT(float* input, const size_t width, const size_t height);
};

#endif /* directDCT_hpp */
