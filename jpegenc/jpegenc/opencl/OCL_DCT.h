#ifndef OCL_DCT_hpp
#define OCL_DCT_hpp

#include <stdlib.h>

class OCL_DCT {
public:
	static void separated(float* &matrix, size_t width, size_t height);
	static void arai(float* &matrix, size_t width, size_t height);
};

#endif /* OCL_DCT_hpp */
