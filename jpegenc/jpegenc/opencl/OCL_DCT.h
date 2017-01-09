#ifndef OCL_DCT_hpp
#define OCL_DCT_hpp

#include <stdlib.h>

class OCL_DCT {
public:
	static void prepareOpenCL();
	static void separated(float* &matrix, size_t width, size_t height);
	
private:
};

#endif /* OCL_DCT_hpp */
