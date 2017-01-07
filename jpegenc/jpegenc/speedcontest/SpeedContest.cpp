#include "SpeedContest.hpp"
#include "Performance.hpp"
#include "Arai.hpp"
#include "DCT.hpp"
#include "OCL_DCT.h"
#include "GPUComposer.h"
#include <thread>

// 1, 7, 3, 4, 5, 4, 3, 2
// one way transform gets:
// 10.253, 0.797218, -2.19761, -0.0377379, -1.76777, -2.75264, -2.53387, -1.13403

void runCPUSingleCore(float* &matrix, size_t width, size_t height, size_t seconds) {
	size_t copySize = width * height * sizeof(float);
	float* vls = new float[width * height];
	float *out = new float[width * height];
	
	memcpy(vls, matrix, copySize);
	Performance::howManyOperationsInSeconds(seconds, "Normal DCT", [&]{
		DCT::transform(vls, out, width, height);
		vls = out;
	});
	delete [] out;
	
	memcpy(vls, matrix, copySize);
	Performance::howManyOperationsInSeconds(seconds, "Separated DCT", [&]{
		DCT::transform2(vls, width, height);
	});
	
	memcpy(vls, matrix, copySize);
	Performance::howManyOperationsInSeconds(seconds, "Arai inline transpose", [&]{
		Arai::transformInlineTranspose(vls, width, height);
	});
	
	memcpy(vls, matrix, copySize);
	Performance::howManyOperationsInSeconds(seconds, "Arai DCT", [&]{
		Arai::transform(vls, width, height);
	});
}

void runCPUMultiCore(float* &matrix, size_t width, size_t height, size_t seconds) {
	size_t copySize = width * height * sizeof(float);
	float* vls = new float[width * height];
	float *out = new float[width * height];
	
	memcpy(vls, matrix, copySize);
	Performance::howManyOperationsInSeconds(seconds, "Normal DCT", [&]{
		DCT::transform(vls, out, width, height);
		vls = out;
	}, true);
	delete [] out;
	
	memcpy(vls, matrix, copySize);
	Performance::howManyOperationsInSeconds(seconds, "Separated DCT", [&]{
		DCT::transform2(vls, width, height);
	}, true);
	
	memcpy(vls, matrix, copySize);
	Performance::howManyOperationsInSeconds(seconds, "Arai inline transpose", [&]{
		Arai::transformInlineTranspose(vls, width, height);
	}, true);
	
	memcpy(vls, matrix, copySize);
	Performance::howManyOperationsInSeconds(seconds, "Arai DCT", [&]{
		Arai::transform(vls, width, height);
	}, true);
}

void runGPU(float* &matrix, size_t width, size_t height, size_t seconds) {
	float* vls = new float[width * height];
	size_t copySize = width * height * sizeof(float);
	
	memcpy(vls, matrix, copySize);
	OCL_DCT::separated(vls, width, height); // once to compile GPU kernel
	
	memcpy(vls, matrix, copySize);
	Performance::howManyOperationsInSeconds(seconds, "Separated (Single Image)", [&]{ OCL_DCT::separated(vls, width, height); });
	
	memcpy(vls, matrix, copySize);
	GPUComposer composer = GPUComposer(OCL_DCT::separated);
	Performance::howManyOperationsInSeconds(seconds, "Separated (Multi Image Composer)", [&]{
		if (composer.add(vls, width, height)) {
			composer.flush(); // send to GPU
			//printf("processing %lu simultaniously\n",comp.cacheInfo.size());
			// do something with the data
			//composer.cacheInfo[i]
			//composer.cache
		}
	});
}

void SpeedContest::run(size_t seconds, float* &matrix, size_t width, size_t height) {
	printf("\nSingle-Threaded:\n");
	runCPUSingleCore(matrix, width, height, seconds);
	
	printf("\nMulti-Threading:\n");
	runCPUMultiCore(matrix, width, height, seconds);
	
	printf("\nGPU:\n");
	runGPU(matrix, width, height, seconds);
	
	printf("\n\n");
}
