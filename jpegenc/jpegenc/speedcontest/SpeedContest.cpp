#include "SpeedContest.hpp"
#include <stdlib.h>
#include <thread>
#include <math.h>
#include "../helper/Performance.hpp"
#include "../dct/Arai.hpp"
#include "../dct/DCT.hpp"
#include "../opencl/OCL_DCT.h"
#include "../opencl/GPUComposer.h"
#include "../opencl/OCLManager.hpp"

// 1, 7, 3, 4, 5, 4, 3, 2
// one way transform gets:
// 10.253, 0.797218, -2.19761, -0.0377379, -1.76777, -2.75264, -2.53387, -1.13403

//  ---------------------------------------------------------------
// |
// |  Helper
// |
//  ---------------------------------------------------------------

float* createTestMatrix(size_t width, size_t height) {
	float *data = new float[width * height];
	for (size_t y = 0; y < height; y++) {
		for (size_t x = 0; x < width; x++) {
			data[y * width + x]= (float)((x+y*8) % 256);
		}
	}
	return data; // remember to delete[]
}

float* createOurTestMatrix(size_t w, size_t h) {
	float data[8] = {1, 7, 3, 4, 5, 4, 3, 2}; // generate our well known test matrix
	float *vls = new float[w * h];
	for (size_t y = 0; y < h; y++) {
		for (size_t x = 0; x < w; x++) {
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
	for (size_t i = 0; i < w * h; ++i) {
		if (i % (w*8) == 0) printf("\n");
		if (mat[i] < 0.0005F && mat[i] > -0.0005F) printf("%8d ", 0);
		else printf("%8.3f ", mat[i]);
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

void verify(const char* desc, float* originalMatrix, size_t width, size_t height, std::function<float*(float*)> func) {
	size_t size = width * height;
	float *vls = new float[size];
	copyArray(vls, originalMatrix, size);
	float *result = func(vls);
	
	printf("\n%s\n", desc);
	printFloatMatrix(result, width, height);
	printf("------------------------------------------------------------> ");
	
	// Check if result is correct
	bool inverseIsCorrect = true;
	float *out = new float[size];
	DCT::inverse(result, out, width, height);
	while (size--) {
		if (fabsf(originalMatrix[size] - out[size]) > 0.0005F )
			inverseIsCorrect = false;
	}
	delete [] vls;
	
	// Break on Error
	if (inverseIsCorrect) {
		printf("Inverse: CORRECT\n");
	} else {
		printf("Inverse: FAILED\nInverse:\n");
		printFloatMatrix(out, width, height);
		exit(EXIT_FAILURE);
	}
	delete [] out;
}

// ################################################################
// #
// #  CPU - Single Core
// #
// ################################################################

void runCPUSingleCore(float* &matrix, size_t width, size_t height, double seconds) {
	size_t size = width * height;
	float* vls = new float[size];
	float* out = new float[size];
	
	copyArray(vls, matrix, size);
	Performance::howManyOperationsInSeconds(seconds, "Normal DCT", [&]{
		DCT::transform(vls, out, width, height);
	});
	delete [] out;
	
	copyArray(vls, matrix, size);
	Performance::howManyOperationsInSeconds(seconds, "Separated DCT", [&]{
		DCT::transform2(vls, width, height);
	});
	
	copyArray(vls, matrix, size);
	Performance::howManyOperationsInSeconds(seconds, "Arai inline transpose", [&]{
		Arai::transformInlineTranspose(vls, width, height);
	});
	
	copyArray(vls, matrix, size);
	Performance::howManyOperationsInSeconds(seconds, "Arai DCT", [&]{
		Arai::transform(vls, width, height);
	});
	
	delete [] vls;
}

// ################################################################
// #
// #  CPU - Multi Core
// #
// ################################################################

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

// ################################################################
// #
// #  GPU
// #
// ################################################################

void testGPUSingleFunction(const char* desc, float* matrix, size_t width, size_t height, double seconds, gpu_function func) {
	float* vls = new float[width * height];
	copyArray(vls, matrix, width * height);
	size_t iterations = 0;
	Timer t;
	while (t.elapsed() < seconds) {
		func(vls, width, height);
		++iterations;
	}
	double time = t.elapsed();
	PerformancePrintOperationsPerSecond(desc, time, iterations);
	delete [] vls;
}

void testGPUComposer(const char* desc, float* matrix, size_t width, size_t height, double seconds, gpu_function func, bool varSize) {
	float* vls = new float[width * height];
	copyArray(vls, matrix, width * height);
	GPUComposer composer = GPUComposer(func, varSize);
	size_t iterations = 0;
	Timer t;
	while (t.elapsed() < seconds) {
		if (composer.add(vls, width, height)) {
			composer.flush(); // send to GPU
			iterations += composer.cacheInfo.size();
			// do something with the data
		}
	}
	composer.flush(); // send remaining images to GPU
	iterations += composer.cacheInfo.size();
	double time = t.elapsed();
	PerformancePrintOperationsPerSecond(desc, time, iterations);
	delete [] vls;
}

void runGPU(float* &matrix, size_t width, size_t height, double seconds) {
	OCLManager::printDevices();
	
	float* vls = new float[width * height];
	copyArray(vls, matrix, width * height);
	OCL_DCT::separated(vls, width, height); // once to init static var ocl
	delete [] vls;
	
	testGPUSingleFunction("Normal DCT (Single Image)", matrix, width, height, seconds, OCL_DCT::normal);
	testGPUSingleFunction("Separated DCT (Single Image)", matrix, width, height, seconds, OCL_DCT::separated);
	testGPUSingleFunction("Arai (Single Image)", matrix, width, height, seconds, OCL_DCT::arai);
	
	testGPUComposer("Separated (Composer, var. Size)", matrix, width, height, seconds, OCL_DCT::separated, false);
	testGPUComposer("Separated (Composer, Same Size)", matrix, width, height, seconds, OCL_DCT::separated, true);
}

// ################################################################
// #
// #  Main
// #
// ################################################################

void SpeedContest::run(double seconds, bool skipCPU, bool skipGPU) {
	size_t width = 256, height = 256;
	float *matrix = createTestMatrix(width, height); // (x+y*8) % 256;
	
	if (skipCPU == false) {
		printf("\n== Single-Threaded ==\n");
		runCPUSingleCore(matrix, width, height, seconds);
		
		printf("\n== Multi-Threading ==\n");
		printf("Threads: %d\n", std::thread::hardware_concurrency());
		runCPUMultiCore(matrix, width, height, seconds);
	}
	
	if (skipGPU == false && OCLManager::hasValidDevice()) {
		printf("\n== GPU ==\n");
		runGPU(matrix, width, height, seconds);
	}
	
	printf("\n");
	
	delete [] matrix;
}

// ################################################################
// #
// #  Correctness Test
// #
// ################################################################

void SpeedContest::testForCorrectness(bool ourTestMatrix, bool use16x16, bool modifyData) {
	size_t width = 8, height = 8;
	if (use16x16) {
		width = 16; height = 16;
	}
	
	float* matrix = ( ourTestMatrix
					 ? createOurTestMatrix(width, height) // 1, 7, 3, 4, 5, 4, 3, 2
					 : createTestMatrix(width, height) );
	
	if (use16x16 && modifyData) {
		// modify some values to get different results, especially for 16 x 16
		matrix[8] = 0;
		matrix[8+1] = 4;
		matrix[8+width] = 4;
		matrix[2*width + 1] = 8;
		matrix[12*width + 1] = 1;
	}
	
	printf("\nInput:\n");
	printFloatMatrix(matrix, width, height);
	printf("------------------------------------------------------------------------\n");
	
	float *out = new float[width * height];
	verify("DCT Normal", matrix, width, height, [&](float* mat){
		DCT::transform(mat, out, width, height); return out;
	});
	verify("DCT Separated", matrix, width, height, [&](float* mat){
		DCT::transform2(mat, width, height); return mat;
	});
	verify("Arai", matrix, width, height, [&](float* mat){
		Arai::transform(mat, width, height); return mat;
	});
	verify("Arai Inline Transpose", matrix, width, height, [&](float* mat){
		Arai::transformInlineTranspose(mat, width, height); return mat;
	});
	
	if (OCLManager::hasValidDevice()) {
		verify("GPU Normal", matrix, width, height, [&](float* mat){
			OCL_DCT::normal(mat, width, height); return mat;
		});
		verify("GPU Separated", matrix, width, height, [&](float* mat){
			OCL_DCT::separated(mat, width, height); return mat;
		});
		verify("GPU Arai", matrix, width, height, [&](float* mat){
			OCL_DCT::arai(mat, width, height); return mat;
		});
		
		GPUComposer c = GPUComposer(OCL_DCT::separated, true); // with different sizes
		verify("GPU Composer", matrix, width, height, [&](float* mat){
			c.add(mat, width, height);
			c.add(mat, width, height);
			c.flush(); // send to GPU
			float* dataPtr = &c.cache[c.cacheInfo[1].offset];
			return dataPtr;
		});
	}
	
	printf("\nInverse:\n");
	DCT::transform2(matrix, width, height);
	DCT::inverse(matrix, out, width, height);
	printFloatMatrix(out, width, height);
	printf("-----------------------------------------------------------------> ALL CORRECT\n");
	delete [] out;
}
