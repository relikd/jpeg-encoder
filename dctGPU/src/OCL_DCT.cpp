#include "OCL_DCT.h"
#include "oclAssert.h"
#include "OCLManager.hpp"

#define BLOCK_DIM 8 // same block size like cl kernel

static const size_t localWorkSize[2] = {BLOCK_DIM, BLOCK_DIM};
const size_t local_mem_size = (BLOCK_DIM + 1) * BLOCK_DIM * sizeof(float);

static OCLManager* ocl = nullptr;
static cl_kernel normalKernel;
static cl_kernel separatedKernel;
static cl_kernel araiKernel;

void initDCT() {
	cl_int errcode = CL_SUCCESS;
	ocl = new OCLManager("arai.cl");
	
	// Check wether the device can handle the cl program
	cl_uint dimensions;
	clGetDeviceInfo(ocl->device, CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS, sizeof(cl_uint), &dimensions, NULL);
	size_t* workSize = new size_t[dimensions];
	clGetDeviceInfo(ocl->device, CL_DEVICE_MAX_WORK_ITEM_SIZES, dimensions * sizeof(size_t), workSize, NULL);
	if (workSize[0] < BLOCK_DIM || workSize[1] < BLOCK_DIM) {
		fputs("Error: Device doesn't support a local worksize of 8x8 which is required for the cl kernel\n", stderr);
		exit(EXIT_FAILURE);
	}
	
	// Create kernels
	normalKernel = clCreateKernel(ocl->program, "dct_normal", &errcode);
	oclAssert(errcode);
	separatedKernel = clCreateKernel(ocl->program, "dct_separated", &errcode);
	oclAssert(errcode);
	araiKernel = clCreateKernel(ocl->program, "dct_arai", &errcode);
	oclAssert(errcode);
}

//  ---------------------------------------------------------------
// |
// |  Normal DCT
// |
//  ---------------------------------------------------------------

void OCL_DCT::normal(float* matrix, size_t size_x, size_t size_y) {
	if (ocl == nullptr) {
		initDCT();
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
	errcode |= clSetKernelArg(normalKernel, 2, sizeof(unsigned int), &size_x);
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

//  ---------------------------------------------------------------
// |
// |  Separated DCT
// |
//  ---------------------------------------------------------------

void OCL_DCT::separated(float* matrix, size_t size_x, size_t size_y) {
	if (ocl == nullptr) {
		initDCT();
	}
	
	cl_int errcode = CL_SUCCESS;
	const size_t mem_size = sizeof(float) * size_x * size_y;
	
	// Setup device memory
	cl_mem d_idata = clCreateBuffer(ocl->context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, mem_size, matrix, &errcode);
	oclAssert(errcode);
	cl_mem d_odata = clCreateBuffer(ocl->context, CL_MEM_WRITE_ONLY, mem_size, NULL, &errcode);
	oclAssert(errcode);
	
	// Set parameter values on device
	errcode  = clSetKernelArg(separatedKernel, 0, sizeof(cl_mem), &d_odata);
	errcode |= clSetKernelArg(separatedKernel, 1, sizeof(cl_mem), &d_idata);
	errcode |= clSetKernelArg(separatedKernel, 2, sizeof(unsigned int), &size_x);
	errcode |= clSetKernelArg(separatedKernel, 3, local_mem_size, 0 );
	errcode |= clSetKernelArg(separatedKernel, 4, local_mem_size, 0 );
	errcode |= clSetKernelArg(separatedKernel, 5, local_mem_size, 0 );
	oclAssert(errcode);
	
	// set up execution configuration
	size_t globalWorkSize[2] = {size_x, size_y};
	
	oclAssert( clEnqueueNDRangeKernel(ocl->commandQueue, separatedKernel, 2, NULL, globalWorkSize, localWorkSize, 0, NULL, NULL) );
	
	// Block CPU till GPU is done
	oclAssert( clFinish(ocl->commandQueue) );
	
	// Retrieve result from device
	oclAssert( clEnqueueReadBuffer(ocl->commandQueue, d_odata, CL_TRUE, 0, mem_size, matrix, 0, NULL, NULL) );
	
	errcode |= clReleaseMemObject(d_idata);
	errcode |= clReleaseMemObject(d_odata);
	oclAssert(errcode);
}

//  ---------------------------------------------------------------
// |
// |  Arai
// |
//  ---------------------------------------------------------------

void OCL_DCT::arai(float* matrix, size_t size_x, size_t size_y) {
	if (ocl == nullptr) {
		initDCT();
	}
	
	cl_int errcode = CL_SUCCESS;
	const size_t mem_size = sizeof(float) * size_x * size_y;
	
	// Setup device memory
	cl_mem d_idata = clCreateBuffer(ocl->context, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, mem_size, matrix, &errcode);
	oclAssert(errcode);
	cl_mem d_odata = clCreateBuffer(ocl->context, CL_MEM_READ_WRITE, mem_size, NULL, &errcode);
	oclAssert(errcode);
	
	// set up execution configuration
	size_t globalWorkSize[2] = {size_x, size_y};
	
	// Set parameter values on device
	errcode  = clSetKernelArg(araiKernel, 0, sizeof(cl_mem), &d_odata);
	errcode |= clSetKernelArg(araiKernel, 1, sizeof(cl_mem), &d_idata);
	errcode |= clSetKernelArg(araiKernel, 2, sizeof(unsigned int), &size_x);
	errcode |= clSetKernelArg(araiKernel, 3, local_mem_size, 0 );
	oclAssert(errcode);
	oclAssert( clEnqueueNDRangeKernel(ocl->commandQueue, araiKernel, 2, NULL, globalWorkSize, localWorkSize, 0, NULL, NULL) );
	// requeue for col arai
	errcode  = clSetKernelArg(araiKernel, 0, sizeof(cl_mem), &d_idata);
	errcode |= clSetKernelArg(araiKernel, 1, sizeof(cl_mem), &d_odata);
	oclAssert( clEnqueueNDRangeKernel(ocl->commandQueue, araiKernel, 2, NULL, globalWorkSize, localWorkSize, 0, NULL, NULL) );
	
	// Block CPU till GPU is done
	oclAssert( clFinish(ocl->commandQueue) );
	
	// Retrieve result from device
	oclAssert( clEnqueueReadBuffer(ocl->commandQueue, d_idata, CL_TRUE, 0, mem_size, matrix, 0, NULL, NULL) );
	
	errcode |= clReleaseMemObject(d_idata);
	errcode |= clReleaseMemObject(d_odata);
	oclAssert(errcode);
}

