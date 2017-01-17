#ifndef DCT_Nvidia_hpp
#define DCT_Nvidia_hpp

#include <stdlib.h>

// Source: https://github.com/sschaetz/nvidia-opencl-examples/tree/master/OpenCL/src/oclDCT8x8

class DCT_Nvidia {
public:
	static void arai(float* matrix, size_t width, size_t height);
};

#endif /* DCT_Nvidia_hpp */
