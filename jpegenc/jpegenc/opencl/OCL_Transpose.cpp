#include "OCL_Transpose.h"
#include "oclUtils.h"

#define ENABLE_MULTI_GPU 1
#define BLOCK_DIM 8 // this is the block size which will be transposed

#define MIN(a, b) ((a < b) ? a : b)
#define oclAssert(x) oclCheckError(x, CL_SUCCESS);


// max GPU's to manage for multi-GPU parallel compute
const static size_t MAX_GPU_COUNT = 8;

inline size_t shrRoundUp(int group_size, int global_size) {
	int r = global_size % group_size;
	if (r == 0)
		return global_size;
	else
		return global_size + group_size - r;
}

inline void getSingleContextAndDevice(cl_context &theContext, cl_device_id* &theDevices) {
	cl_int errcode;
	theContext = clCreateContextFromType(0, CL_DEVICE_TYPE_GPU, NULL, NULL, &errcode);
	oclAssert(errcode);
	
	// get the list of GPU devices associated with context
	size_t dataBytes;
	oclAssert( clGetContextInfo(theContext, CL_CONTEXT_DEVICES, 0, NULL, &dataBytes) );
	theDevices = (cl_device_id *) malloc(dataBytes);
	oclAssert( clGetContextInfo(theContext, CL_CONTEXT_DEVICES, dataBytes, theDevices, NULL) );
}

inline cl_uint getContextAndDevices(cl_context &theContext, cl_device_id* &theDevices) {
	//Get the NVIDIA platform
	cl_platform_id cpPlatform;
	oclAssert( oclGetPlatformID(&cpPlatform) );
	
	//Get the devices
	cl_uint uiNumDevices;
	oclAssert( clGetDeviceIDs(cpPlatform, CL_DEVICE_TYPE_GPU, 0, NULL, &uiNumDevices) );
	
	if (uiNumDevices == 0) {
		DebugLog(" There are no devices supporting OpenCL\n\n");
		return 0;
	}
	
	theDevices = (cl_device_id *) malloc( uiNumDevices * sizeof(cl_device_id) );
	oclAssert( clGetDeviceIDs(cpPlatform, CL_DEVICE_TYPE_GPU, uiNumDevices, theDevices, NULL) );
	
	//Create the context
	cl_int errcode;
	theContext = clCreateContext(0, uiNumDevices, theDevices, NULL, NULL, &errcode);
	oclAssert(errcode);
	
	return uiNumDevices;
}

inline cl_program loadProgram(const char *path, cl_context &theContext) {
	// Program setup
	size_t program_length;
	char *source = oclLoadProgSource(path, "", &program_length);
	if (source == NULL) {
		DebugLog("Couldn't open OpenCL source file\n\n");
		exit(EXIT_FAILURE);
	}
	
	// Create the program
	cl_int errcode;
	cl_program prog = clCreateProgramWithSource(theContext, 1, (const char **)&source, &program_length, &errcode);
	free(source);
	oclAssert(errcode);
	
	// Build the program
	errcode = clBuildProgram(prog, 0, NULL, "-cl-fast-relaxed-math", NULL, NULL);
	if (errcode != CL_SUCCESS) {
		// write out standard error, Build Log and PTX, then return error
		DebugLog("file %s, line %i\n\n" , __FILE__ , __LINE__);
		oclLogBuildInfo(prog, oclGetFirstDev(theContext));
		oclLogPtx(prog, oclGetFirstDev(theContext), "temporaryDebug.ptx");
		return 0;
	}
	return prog;
}

void computeOnGPU(const char* kernelName, float* h_idata, unsigned int size_x, unsigned int size_y) {
	// Setup Context
	cl_int errcode = CL_SUCCESS;
	cl_context clGPUContext;
	cl_device_id* devIDs;
	
#if ENABLE_MULTI_GPU
	const cl_uint GPU_COUNT = getContextAndDevices(clGPUContext, devIDs);
#else
	const cl_uint GPU_COUNT = 1;
	getSingleContextAndDevice(clGPUContext, devIDs);
#endif
	
	
	// Create a command-queue
	cl_command_queue commandQueue[GPU_COUNT];
	for (unsigned short id = 0; id < GPU_COUNT; ++id) {
		// get and print the device for this queue
		DebugLog("Device %d: ", id);
		oclPrintDevName(devIDs[id]);
		DebugLog("\n");
		
		commandQueue[id] = clCreateCommandQueue(clGPUContext, devIDs[id], CL_QUEUE_PROFILING_ENABLE, &errcode); // CL_QUEUE_PROFILING_ENABLE
		oclAssert(errcode);
	}
	free(devIDs);
	
	
	// Create Program / Kernel
	cl_program clProgram = loadProgram("../jpegenc/opencl/transpose.cl", clGPUContext);
	
	// Setup Memory
	cl_mem d_idata[GPU_COUNT];
	cl_mem d_odata[GPU_COUNT];
	cl_kernel clKernel[GPU_COUNT];
	
	const size_t mem_size_in = sizeof(float) * size_x * size_y;
	size_t sizePerGPU = shrRoundUp(BLOCK_DIM, (size_x + GPU_COUNT - 1) / GPU_COUNT);
	const size_t mem_size_out = sizeof(float) * size_y * sizePerGPU;
	
	for (unsigned short i = 0; i < GPU_COUNT; ++i) {
		// Setup device memory
		d_idata[i] = clCreateBuffer(clGPUContext, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, mem_size_in, h_idata, &errcode);
		oclAssert(errcode);
		d_odata[i] = clCreateBuffer(clGPUContext, CL_MEM_WRITE_ONLY, mem_size_out, NULL, &errcode);
		oclAssert(errcode);
		
		// Create kernel
		clKernel[i] = clCreateKernel(clProgram, kernelName, &errcode);
		oclAssert(errcode);
		
		// Launch OpenCL kernel
		size_t offset = i * sizePerGPU;
		errcode  = clSetKernelArg(clKernel[i], 0, sizeof(cl_mem), (void *) &d_odata[i]);
		errcode |= clSetKernelArg(clKernel[i], 1, sizeof(cl_mem), (void *) &d_idata[i]);
		errcode |= clSetKernelArg(clKernel[i], 2, sizeof(int), &offset);
		errcode |= clSetKernelArg(clKernel[i], 3, sizeof(int), &size_x);
		errcode |= clSetKernelArg(clKernel[i], 4, sizeof(int), &size_y);
		errcode |= clSetKernelArg(clKernel[i], 5, (BLOCK_DIM + 1) * BLOCK_DIM * sizeof(float), 0 );
		oclAssert(errcode);
	}

	
	// set up execution configuration
	size_t localWorkSize[2], globalWorkSize[2];
	localWorkSize[0] = BLOCK_DIM;
	localWorkSize[1] = BLOCK_DIM;
	globalWorkSize[0] = sizePerGPU;
	globalWorkSize[1] = shrRoundUp(BLOCK_DIM, size_y);
	
	
	DebugLog("\nProcessing a %d by %d matrix of floats...\n\n", size_x, size_y);
	for (unsigned int k = 0; k < GPU_COUNT; ++k) {
		errcode |= clEnqueueNDRangeKernel(commandQueue[k], clKernel[k], 2, NULL,
										  globalWorkSize, localWorkSize, 0, NULL, NULL);
	}
	oclAssert(errcode);
	
	
	// Block CPU till GPU is done
	for (unsigned int q = 0; q < GPU_COUNT; ++q) {
		errcode |= clFinish(commandQueue[q]);
	}
	oclAssert(errcode);
	
	
	// Retrieve result from device
	for (unsigned int i = 0; i < GPU_COUNT; ++i) {
		size_t offset = i * sizePerGPU;
		size_t size = MIN(size_x - offset, sizePerGPU) * size_y * sizeof(float);
		
		errcode |= clEnqueueReadBuffer(commandQueue[i], d_odata[i], CL_TRUE, 0, size, &h_idata[offset * size_y], 0, NULL, NULL);
	}
	oclAssert(errcode);
	
	// Cleanup Open CL
	clReleaseContext(clGPUContext);
	clReleaseProgram(clProgram);
	
	for (unsigned int i = 0; i < GPU_COUNT; ++i) {
		errcode |= clReleaseMemObject(d_idata[i]);
		errcode |= clReleaseMemObject(d_odata[i]);
		errcode |= clReleaseKernel(clKernel[i]);
		errcode |= clReleaseCommandQueue(commandQueue[i]);
	}
	oclAssert(errcode);
}

void OCL_Transpose::transpose8x8(Channel* &a) {
	computeOnGPU("transpose", a->values, (unsigned int)a->imageSize.width, (unsigned int)a->imageSize.height);
}
