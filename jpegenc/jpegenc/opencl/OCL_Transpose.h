#ifndef OCL_Transpose_hpp
#define OCL_Transpose_hpp

#include "Channel.hpp"

class OCL_Transpose {
public:
	void transpose8x8(Channel* &a);
	
private:
//	size_t shrRoundUp(int group_size, int global_size);
//	void transposeCL(const char* kernelName, bool useLocalMem, cl_uint ciDeviceCount, float* h_idata, float* h_odata, unsigned int size_x, unsigned int size_y);
};

#endif /* OCL_Transpose_hpp */
