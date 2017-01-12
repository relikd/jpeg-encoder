#include "SpeedContest.hpp"
#include <stdlib.h>
#include <thread>
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

//  ---------------------------------------------------------------
// |
// |  Performance
// |
//  ---------------------------------------------------------------

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
	
	OCLManager::printDevices();
	copyArray(vls, matrix, size);
	OCL_DCT::separated(vls, width, height); // once to init static var ocl
	
	Timer t;
	double time;
	size_t iterations;
	
	// BEGIN: "Separated (Single Image)"
	{
		copyArray(vls, matrix, size);
		iterations = 0;
		t.reset();
		while (t.elapsed() < seconds) {
			OCL_DCT::separated(vls, width, height);
			++iterations;
		}
		time = t.elapsed();
		PerformancePrintOperationsPerSecond("Separated (Single Image)", time, iterations);
	}
	
	// BEGIN: "Separated (Composer, var. Size)"
	{
		copyArray(vls, matrix, size);
		GPUComposer c1 = GPUComposer(OCL_DCT::separated, false);
		iterations = 0;
		t.reset();
		while (t.elapsed() < seconds) {
			if (c1.add(vls, width, height)) {
				c1.flush(); // send to GPU
				iterations += c1.cacheInfo.size();
				// do something with the data
				// c1.cacheInfo[i], c1.cache
			}
		}
		c1.flush(); // send remaining images to GPU
		iterations += c1.cacheInfo.size();
		time = t.elapsed();
		PerformancePrintOperationsPerSecond("Separated (Composer, var. Size)", time, iterations);
	}
	
	// BEGIN: "Separated (Composer, Same Size)"
	{
		copyArray(vls, matrix, size);
		GPUComposer c2 = GPUComposer(OCL_DCT::separated, true);
		iterations = 0;
		t.reset();
		while (t.elapsed() < seconds) {
			if (c2.add(vls, width, height)) {
				c2.flush(); // send to GPU
				iterations += c2.cacheInfo.size();
				// do something with the data
			}
		}
		c2.flush(); // send remaining images to GPU
		iterations += c2.cacheInfo.size();
		time = t.elapsed();
		PerformancePrintOperationsPerSecond("Separated (Composer, Same Size)", time, iterations);
	}
	
	// BEGIN: "Arai (Composer, Same Size)"
	{
		copyArray(vls, matrix, size);
		GPUComposer c3 = GPUComposer(OCL_DCT::arai, true);
		iterations = 0;
		t.reset();
		while (t.elapsed() < seconds) {
			if (c3.add(vls, width, height)) {
				c3.flush(); // send to GPU
				iterations += c3.cacheInfo.size();
				// do something with the data
			}
		}
		c3.flush(); // send remaining images to GPU
		iterations += c3.cacheInfo.size();
		time = t.elapsed();
		PerformancePrintOperationsPerSecond("Arai (Composer, Same Size)", time, iterations);
	}
	
	delete [] vls;
}

//  ---------------------------------------------------------------
// |
// |  Main
// |
//  ---------------------------------------------------------------

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
	
	if (skipGPU == false) {
		printf("\n== GPU ==\n");
		runGPU(matrix, width, height, seconds);
	}
	
	printf("\n");
	
	delete [] matrix;
}

void SpeedContest::testForCorrectness(bool ourTestMatrix, bool use16x16, bool modifyData) {
	size_t width = 8, height = 8;
	if (use16x16) {
		width = 16; height = 16;
	}
	size_t size = width * height;
	float* matrix;
	
	if (ourTestMatrix) {
		matrix = createOurTestMatrix(width, height); // 1, 7, 3, 4, 5, 4, 3, 2
	} else {
		matrix = createTestMatrix(width, height);
	}
	
	if (use16x16 && modifyData) {
		// modify some values to get different results, especially for 16 x 16
		matrix[8] = 0;
		matrix[8+1] = 4;
		matrix[8+width] = 4;
		matrix[2*width + 1] = 8;
		matrix[12*width + 1] = 1;
	}

	
	float *vls = new float[width * height];
	float *out = new float[width * height];
	
	printf("\nInput:\n");
	printFloatMatrix(matrix, width, height);
	printf("------------------------------------------------------------------------\n");
	
	copyArray(vls, matrix, size);
	
	// normal
	printf("\nDCT Normal:\n");
	DCT::transform(vls, out, width, height);
	printFloatMatrix(out, width, height);
	printf("------------------------------------------------------------------------\n");
	
	copyArray(vls, matrix, size);
	
	// separated
	printf("\nDCT Separated:\n");
	DCT::transform2(vls, width, height);
	printFloatMatrix(vls, width, height);
	printf("------------------------------------------------------------------------\n");
	
	copyArray(vls, matrix, size);
	
	// arai
	printf("\nArai:\n");
	Arai::transform(vls, width, height);
	printFloatMatrix(vls, width, height);
	printf("------------------------------------------------------------------------\n");
	
	copyArray(vls, matrix, size);
	
	// arai inline transpose
	printf("\nArai Inline Transpose:\n");
	Arai::transformInlineTranspose(vls, width, height);
	printFloatMatrix(vls, width, height);
	printf("------------------------------------------------------------------------\n");
	
	copyArray(vls, matrix, size);
	
	// GPU arai
	printf("\nGPU Arai:\n");
	OCL_DCT::arai(vls, width, height);
	printFloatMatrix(vls, width, height);
	printf("------------------------------------------------------------------------\n");
	
	copyArray(vls, matrix, size);
	
	// GPU separated
	printf("\nGPU Separated:\n");
	OCL_DCT::separated(vls, width, height);
	printFloatMatrix(vls, width, height);
	printf("------------------------------------------------------------------------\n");
	
	copyArray(vls, matrix, size);
	
	// GPU Composer
	printf("\nGPU Composer:\n");
	GPUComposer c = GPUComposer(OCL_DCT::separated, true); // with different sizes
	c.add(vls, width, height);
	c.add(vls, width, height);
	c.flush(); // send to GPU
	float* dataPtr = &c.cache[c.cacheInfo[1].offset];
	printFloatMatrix(dataPtr, c.cacheInfo[1].width, c.cacheInfo[1].height);
	printf("------------------------------------------------------------------------\n");
	
	printf("\nInverse:\n");
	DCT::inverse(c.cache, out, width, height);
	printFloatMatrix(out, width, height);
	printf("------------------------------------------------------------------------\n");
	
	delete [] out;
	delete [] vls;
}
