#include <iostream>
#include <stdlib.h>
#include <thread>
#include <math.h>
#include <functional>
#include <chrono>
#include "OCL_DCT.h"
#include "GPUComposer.h"
#include "OCLManager.hpp"


#define PerformancePrintOperationsPerSecond(__desc, __time, __count) \
printf("<%s> took %lfsec with %lu iterations (%lfms per operation)\n", __desc, __time, (unsigned long)__count, (__time / __count) * 1000);

class Timer {
public:
	Timer() : beg_(clock_::now()) {}
	void reset() { beg_ = clock_::now(); }
	double elapsed() const {
		return std::chrono::duration_cast<second_>
		(clock_::now() - beg_).count(); }
	
private:
	typedef std::chrono::high_resolution_clock clock_;
	typedef std::chrono::duration<double, std::ratio<1> > second_;
	std::chrono::time_point<clock_> beg_;
};

#define N 8
// 1, 7, 3, 4, 5, 4, 3, 2
// one way transform gets:
// 10.253, 0.797218, -2.19761, -0.0377379, -1.76777, -2.75264, -2.53387, -1.13403

//  ---------------------------------------------------------------
// |
// |  Inverse
// |
//  ---------------------------------------------------------------

static const float constantC[2][2] = { // (2.0F / N) * getC(i) * getC(j)
	{ 0.125F, 0.176776695296636893184327732342353556305170059204101562F },
	{ 0.176776695296636893184327732342353556305170059204101562F, 0.25F }
};

// float %1.25f  double %1.54f
static const float cos_2x1iPi_2N[8][8] = { // cos( (2x + 1)iπ / 2N )
	{ 1,  0.980785280403230449126182236134239036973933730893336095003F,  0.923879532511286756128183189396788286822416625863642486116F,  0.831469612302545237078788377617905756738560811987249963448F,  0.707106781186547524400844362104849039284835937688474036591F,  0.555570233019602224742830813948532874374937190754804045928F,  0.38268343236508977172845998403039886676134456248562704144F,   0.19509032201612826784828486847702224092769161775195480776F },
	{ 1,  0.831469612302545237078788377617905756738560811987249963447F,  0.38268343236508977172845998403039886676134456248562704144F,  -0.19509032201612826784828486847702224092769161775195480775F,  -0.70710678118654752440084436210484903928483593768847403659F,  -0.980785280403230449126182236134239036973933730893336095002F, -0.92387953251128675612818318939678828682241662586364248612F,  -0.55557023301960222474283081394853287437493719075480404593F },
	{ 1,  0.555570233019602224742830813948532874374937190754804045925F, -0.38268343236508977172845998403039886676134456248562704143F,  -0.980785280403230449126182236134239036973933730893336095003F, -0.70710678118654752440084436210484903928483593768847403659F,   0.19509032201612826784828486847702224092769161775195480775F,   0.92387953251128675612818318939678828682241662586364248611F,   0.83146961230254523707878837761790575673856081198724996345F },
	{ 1,  0.195090322016128267848284868477022240927691617751954807755F, -0.923879532511286756128183189396788286822416625863642486115F, -0.555570233019602224742830813948532874374937190754804045925F,  0.707106781186547524400844362104849039284835937688474036587F,  0.831469612302545237078788377617905756738560811987249963448F, -0.382683432365089771728459984030398866761344562485627041431F, -0.980785280403230449126182236134239036973933730893336095004F },
	{ 1, -0.195090322016128267848284868477022240927691617751954807754F, -0.923879532511286756128183189396788286822416625863642486115F,  0.555570233019602224742830813948532874374937190754804045923F,  0.707106781186547524400844362104849039284835937688474036589F, -0.831469612302545237078788377617905756738560811987249963445F, -0.382683432365089771728459984030398866761344562485627041436F,  0.980785280403230449126182236134239036973933730893336095002F },
	{ 1, -0.555570233019602224742830813948532874374937190754804045924F, -0.382683432365089771728459984030398866761344562485627041434F,  0.980785280403230449126182236134239036973933730893336095003F, -0.707106781186547524400844362104849039284835937688474036588F, -0.19509032201612826784828486847702224092769161775195480776F,   0.923879532511286756128183189396788286822416625863642486116F, -0.831469612302545237078788377617905756738560811987249963445F },
	{ 1, -0.831469612302545237078788377617905756738560811987249963446F,  0.382683432365089771728459984030398866761344562485627041434F,  0.195090322016128267848284868477022240927691617751954807755F, -0.707106781186547524400844362104849039284835937688474036589F,  0.980785280403230449126182236134239036973933730893336095003F, -0.923879532511286756128183189396788286822416625863642486115F,  0.55557023301960222474283081394853287437493719075480404592F },
	{ 1, -0.980785280403230449126182236134239036973933730893336095003F,  0.923879532511286756128183189396788286822416625863642486115F, -0.831469612302545237078788377617905756738560811987249963446F,  0.707106781186547524400844362104849039284835937688474036588F, -0.55557023301960222474283081394853287437493719075480404592F,   0.38268343236508977172845998403039886676134456248562704143F,  -0.19509032201612826784828486847702224092769161775195480775F }
};

void inverse8x8(float* input, float* output, const size_t width) {
	unsigned char i,j,x,y;
	x = N;
	while (x--) { // outer loop over output
		y = N;
		while (y--) {
			float inner = 0;
			i = N;
			while (i--) { // inner loop over input
				j = N;
				while (j--) {
					float praefix = constantC[i>0][j>0] * input[j + i * width];
					inner += praefix * cos_2x1iPi_2N[x][i] * cos_2x1iPi_2N[y][j];
				}
			}
			output[y + x * width] = inner;
		}
	}
}

void inverse(float* input, float* output, const size_t width, const size_t height) {
	size_t y = height / N;
	while (y--) {
		size_t x = width / N;
		while (x--) {
			size_t offset = y * width * N + x * N;
			inverse8x8(&input[offset], &output[offset], width);
		}
	}
}


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
	inverse(result, out, width, height);
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
// #  Contest
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

void contest(double seconds) {
	size_t width = 256, height = 256;
	float *matrix = createTestMatrix(width, height); // (x+y*8) % 256;
	
	if (OCLManager::hasValidDevice()) {
		printf("\n== GPU ==\n");
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
	
	printf("\n");
	delete [] matrix;
}

// ################################################################
// #
// #  Correctness Test
// #
// ################################################################

void testForCorrectness(bool ourTestMatrix, bool use16x16, bool modifyData) {
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
			
			printf("\nInverse:\n");
			inverse(dataPtr, out, width, height);
			printFloatMatrix(out, width, height);
			printf("-----------------------------------------------------------------> ALL CORRECT\n");
			
			return dataPtr;
		});
	}
	
	delete [] out;
}


// ################################################################
// #
// #  Main
// #
// ################################################################

int main(int argc, const char *argv[])
{
	double testTime = 10.0F;
	
	int i = argc;
	while (--i) { // skip the first param, which is the path of this executable
		const char* param = argv[i];
		if (param[0] == '-') {
			if (strncmp(param, "-valid", 6) == 0) // -valid111 (three 1 for three bool parameter)
			{
				long validateParam = strtol(param + 6, NULL, 2);
				testForCorrectness((bool)(validateParam & 1), (bool)(validateParam & 2), (bool)(validateParam & 4));
				exit(EXIT_SUCCESS);
			}
			else if (strncmp(param, "-gpu", 4) == 0) // -gpu0, -gpu1, -gpu = show list for selection
			{
				char c = param[4];
				if (c >= '0' && c <= '9') { // select specific gpu
					long gpu = -1;
					gpu = strtol(param + 4, NULL, 10);
					OCLManager::setPreferedGPU((int)gpu);
				} else {
					OCLManager::askUserToSelectGPU();
				}
			}
		} else {
			double tmp = strtod(param, NULL); // see if time provided, otherwise default to 10
			if (tmp > 0.5) testTime = tmp;
		}
	}
	
	printf("Starting Performance Test with %1.1fs\n", testTime);
	contest(testTime);
	
	return 0;
}
