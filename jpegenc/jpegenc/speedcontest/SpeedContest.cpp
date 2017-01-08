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

#define PerformancePerSecond(__sec, __desc, __timer, __time, __count, __block) \
__count = 0; \
__timer.reset(); \
while (__timer.elapsed() < __sec) { __block; } \
__time = __timer.elapsed(); \
PerformancePrintOperationsPerSecond(__desc, __time, __count);

inline void copyArray(float* dst, float* src, size_t size) {
	while (size--) {
		*(dst++) = *(src++);
	}
}

void runCPUSingleCore(float* &matrix, size_t width, size_t height, size_t seconds) {
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
}

void runCPUMultiCore(float* &matrix, size_t width, size_t height, size_t seconds) {
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
}

void runGPU(float* &matrix, size_t width, size_t height, size_t seconds) {
	size_t size = width * height;
	float* vls = new float[size];
	
	copyArray(vls, matrix, size);
	OCL_DCT::separated(vls, width, height); // once to compile GPU kernel
	
	
	Timer t;
	double time;
	size_t iterations;
	
	copyArray(vls, matrix, size);
	GPUComposer composer = GPUComposer(OCL_DCT::separated);
	PerformancePerSecond(seconds, "Separated (Single Image)", t, time, iterations, {
		OCL_DCT::separated(vls, width, height);
		++iterations;
	});
	
	copyArray(vls, matrix, size);
	PerformancePerSecond(seconds, "Separated (Multi Image Composer)", t, time, iterations, {
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
