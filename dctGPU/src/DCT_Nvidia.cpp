#include "DCT_Nvidia.h"
#include "oclAssert.h"
#include "OCLManager.hpp"

#define BLOCK_DIM 8 // same block size like cl kernel

static const size_t localWorkSize[2] = {32, 16 / BLOCK_DIM};

static OCLManager* ocl = nullptr;
static cl_kernel normalKernel;

void DCT_Nvidia::arai(float* matrix, size_t size_x, size_t size_y) {
	if (ocl == nullptr) {
		cl_int errcode = CL_SUCCESS;
		ocl = new OCLManager("DCT8x8.cl");
		
		// Check wether the device can handle the cl program
		cl_uint dimensions;
		clGetDeviceInfo(ocl->device, CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS, sizeof(cl_uint), &dimensions, NULL);
		size_t* workSize = new size_t[dimensions];
		clGetDeviceInfo(ocl->device, CL_DEVICE_MAX_WORK_ITEM_SIZES, dimensions * sizeof(size_t), workSize, NULL);
		if (workSize[0] < localWorkSize[0] || workSize[1] < localWorkSize[1]) {
			fputs("Error: Device doesn't support a local worksize of 32x2 which is required for the cl kernel\n", stderr);
			exit(EXIT_FAILURE);
		}
		
		// Create kernels
		normalKernel = clCreateKernel(ocl->program, "DCT8x8", &errcode);
		oclAssert(errcode);
	}
	
	cl_int errcode = CL_SUCCESS;
	const size_t mem_size = sizeof(float) * size_x * size_y;
	
	// Setup device memory
	cl_mem d_idata = clCreateBuffer(ocl->context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, mem_size, matrix, &errcode);
	oclAssert(errcode);
	cl_mem d_odata = clCreateBuffer(ocl->context, CL_MEM_WRITE_ONLY, mem_size, NULL, &errcode);
	oclAssert(errcode);
	
	// Set parameter values on device
	errcode  = clSetKernelArg(normalKernel, 0, sizeof(cl_mem), &d_odata);
	errcode |= clSetKernelArg(normalKernel, 1, sizeof(cl_mem), &d_idata);
	errcode |= clSetKernelArg(normalKernel, 2, sizeof(cl_uint), &size_x);
	errcode |= clSetKernelArg(normalKernel, 3, sizeof(cl_uint), (void*)&size_y);
	errcode |= clSetKernelArg(normalKernel, 4, sizeof(cl_uint), (void*)&size_x);
	oclAssert(errcode);
	
	size_t globalWorkSize[2] = {size_x, size_y};
	oclAssert( clEnqueueNDRangeKernel(ocl->commandQueue, normalKernel, 2, NULL, globalWorkSize, localWorkSize, 0, NULL, NULL) );
	
	// Block CPU till GPU is done
	oclAssert( clFinish(ocl->commandQueue) );
	
	// Retrieve result from device
	oclAssert( clEnqueueReadBuffer(ocl->commandQueue, d_odata, CL_TRUE, 0, mem_size, matrix, 0, NULL, NULL) );
	
	errcode |= clReleaseMemObject(d_idata);
	errcode |= clReleaseMemObject(d_odata);
	oclAssert(errcode);
}
