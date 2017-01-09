#include "SpeedContest.hpp"
#include <stdlib.h>
#include <thread>
#include "../helper/Performance.hpp"
#include "../dtc/Arai.hpp"
#include "../dtc/DCT.hpp"
#include "../opencl/OCL_DCT.h"
#include "../opencl/GPUComposer.h"

// 1, 7, 3, 4, 5, 4, 3, 2
// one way transform gets:
// 10.253, 0.797218, -2.19761, -0.0377379, -1.76777, -2.75264, -2.53387, -1.13403

#define PerformancePerSecond(__sec, __desc, __timer, __time, __count, __block) \
__count = 0; \
__timer.reset(); \
while (__timer.elapsed() < __sec) { __block; } \
__time = __timer.elapsed(); \
PerformancePrintOperationsPerSecond(__desc, __time, __count);

//  ---------------------------------------------------------------
// |
// |  Helper
// |
//  ---------------------------------------------------------------

float* createTestMatrix(size_t width, size_t height) {
	float *data = new float[width * height];
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			data[y * width + x]= (x+y*8) % 256;
		}
	}
	return data; // remember to delete[]
}

float* createOurTestMatrix(size_t w, size_t h) {
	float data[8] = {1, 7, 3, 4, 5, 4, 3, 2}; // generate our well known test matrix
	float *vls = new float[w * h];
	for (int y = 0; y < h; y++) {
		for (int x = 0; x < w; x++) {
			float &u = vls[y * w + x];
			if (y % 8 == 0) {
				u = data[x % 8];
			} else {
				if (x % 8 == 0) {
					u = data[y % 8];
				} else {
					u = 0;
				}
			}
		}
	}
	return vls;
}

void printFloatMatrix(float* &mat, size_t w, size_t h) {
	for (int i = 0; i < w * h; ++i) {
		if (i % (w*8) == 0) printf("\n");
		printf("%1.3f  ", mat[i]);
		if (i % 8 == 7)   printf("   ");
		if (i % w == w-1) printf("\n");
	}
}

inline void copyArray(float* dst, float* src, size_t size) {
	memcpy(dst, src, size * sizeof(float));
//	while (size--) {
//		*(dst++) = *(src++);
//	}
}

//  ---------------------------------------------------------------
// |
// |  Performance
// |
//  ---------------------------------------------------------------

void runCPUSingleCore(float* &matrix, size_t width, size_t height, double seconds) {
	size_t size = width * height;
	float* vls = new float[size];
	float* out = new float[size];
	
	Timer t;
	double time;
	size_t iterations;
	
	copyArray(vls, matrix, size);
	PerformancePerSecond(seconds, "Normal DCT", t, time, iterations, {
		DCT::transform(vls, out, width, height);
		++iterations;
	});
	delete [] out;
	
	copyArray(vls, matrix, size);
	PerformancePerSecond(seconds, "Separated DCT", t, time, iterations, {
		DCT::transform2(vls, width, height);
		++iterations;
	});
	
	copyArray(vls, matrix, size);
	PerformancePerSecond(seconds, "Arai inline transpose", t, time, iterations, {
		Arai::transformInlineTranspose(vls, width, height);
		++iterations;
	});
	
	copyArray(vls, matrix, size);
	PerformancePerSecond(seconds, "Arai DCT", t, time, iterations, {
		Arai::transform(vls, width, height);
		++iterations;
	});
	
	delete [] vls;
}

void runCPUMultiCore(float* &matrix, size_t width, size_t height, double seconds) {
	size_t size = width * height;
	float* vls = new float[size];
	float* out = new float[size];
	
	copyArray(vls, matrix, size);
	Performance::howManyOperationsInSeconds(seconds, "Normal DCT", [&]{
		DCT::transform(vls, out, width, height);
	}, true);
	delete [] out;
	
	copyArray(vls, matrix, size);
	Performance::howManyOperationsInSeconds(seconds, "Separated DCT", [&]{
		DCT::transform2(vls, width, height);
	}, true);
	
	copyArray(vls, matrix, size);
	Performance::howManyOperationsInSeconds(seconds, "Arai inline transpose", [&]{
		Arai::transformInlineTranspose(vls, width, height);
	}, true);
	
	copyArray(vls, matrix, size);
	Performance::howManyOperationsInSeconds(seconds, "Arai DCT", [&]{
		Arai::transform(vls, width, height);
	}, true);
	
	delete [] vls;
}

void runGPU(float* &matrix, size_t width, size_t height, double seconds) {
	size_t size = width * height;
	float* vls = new float[size];
	
	OCL_DCT::prepareOpenCL();
	
	Timer t;
	double time;
	size_t iterations;
	
	copyArray(vls, matrix, size);
	PerformancePerSecond(seconds, "Separated (Single Image)", t, time, iterations, {
		OCL_DCT::separated(vls, width, height);
		++iterations;
	});
	
	copyArray(vls, matrix, size);
	GPUComposer composer = GPUComposer(OCL_DCT::separated);
	PerformancePerSecond(seconds, "Separated (Composer, var. Size)", t, time, iterations, {
		if (composer.add(vls, width, height)) {
			composer.flush(); // send to GPU
			iterations += composer.cacheInfo.size();
			// do something with the data
			//composer.cacheInfo[i]
			//composer.cache
		}
	});
	
	copyArray(vls, matrix, size);
	GPUComposer composerSameSize = GPUComposer(OCL_DCT::separated, true);
	PerformancePerSecond(seconds, "Separated (Composer, Same Size)", t, time, iterations, {
		if (composerSameSize.add(vls, width, height)) {
			composerSameSize.flush(); // send to GPU
			iterations += composerSameSize.cacheInfo.size();
			// do something with the data
		}
	});
	
	copyArray(vls, matrix, size);
	GPUComposer composerSameSizeArai = GPUComposer(OCL_DCT::arai, true);
	PerformancePerSecond(seconds, "Arai (Same Size)", t, time, iterations, {
		if (composerSameSizeArai.add(vls, width, height)) {
			composerSameSizeArai.flush(); // send to GPU
			iterations += composerSameSize.cacheInfo.size();
			// do something with the data
		}
	});
	
	delete [] vls;
}

//  ---------------------------------------------------------------
// |
// |  Main
// |
//  ---------------------------------------------------------------
#define CONTEST_MODE 1
void SpeedContest::run(double seconds)
{
#if CONTEST_MODE
	
	size_t width = 256, height = 256;
	float *matrix = createTestMatrix(width, height);
	
	printf("\nSingle-Threaded:\n");
	runCPUSingleCore(matrix, width, height, seconds);
	
	printf("\nMulti-Threading:\n");
	runCPUMultiCore(matrix, width, height, seconds);
	
	printf("\nGPU:\n");
	runGPU(matrix, width, height, seconds);
	
#else
	
	size_t width = 16, height = 16;
	float *matrix = createOurTestMatrix(width, height);
	matrix[8] = 0;
	matrix[8+1] = 4;
	matrix[8+width] = 4;
	matrix[2*width + 1] = 8;
	matrix[12*width + 1] = 1; // modify some values to get different results
	float *out = new float[width * height];
	DCT::transform(matrix, out, width, height);
	printFloatMatrix(out, width, height);
	delete [] out;
	OCL_DCT::arai(matrix, width, height);
	printFloatMatrix(matrix, width, height);
	
#endif
	
	printf("\n\n");
	
	delete [] matrix;
}
