#include "OCL_DCT.h"
#include "oclUtils.h"

#define BLOCK_DIM 8 // same block size like cl kernel

#define MIN(a, b) ((a < b) ? a : b)
#define oclAssert(x) oclCheckError(x, CL_SUCCESS);

static const float* oclMatrixA = new float[64] {
	0.353553390593273730857504233426880091428756713867187500F,  0.353553390593273730857504233426880091428756713867187500F,  0.353553390593273730857504233426880091428756713867187500F,  0.353553390593273730857504233426880091428756713867187500F,  0.353553390593273730857504233426880091428756713867187500F,  0.353553390593273730857504233426880091428756713867187500F,  0.353553390593273730857504233426880091428756713867187500F,  0.353553390593273730857504233426880091428756713867187500F,
	0.490392640201615215289621119154617190361022949218750000F,  0.415734806151272617835701339572551660239696502685546875F,  0.277785116509801144335511935423710383474826812744140625F,  0.097545161008064151797469776283833198249340057373046875F, -0.097545161008064096286318545026006177067756652832031250F, -0.277785116509800977802058241650229319930076599121093750F, -0.415734806151272673346852570830378681421279907226562500F, -0.490392640201615215289621119154617190361022949218750000F,
	0.461939766255643369241568052530055865645408630371093750F,  0.191341716182544918645191955874906852841377258300781250F, -0.191341716182544863134040724617079831659793853759765625F, -0.461939766255643369241568052530055865645408630371093750F, -0.461939766255643369241568052530055865645408630371093750F, -0.191341716182545168445372496535128448158502578735351562F,  0.191341716182545001911918802761647384613752365112304688F,  0.461939766255643258219265590014401823282241821289062500F,
	0.415734806151272617835701339572551660239696502685546875F, -0.097545161008064096286318545026006177067756652832031250F, -0.490392640201615215289621119154617190361022949218750000F, -0.277785116509801088824360704165883362293243408203125000F,  0.277785116509800922290907010392402298748493194580078125F,  0.490392640201615215289621119154617190361022949218750000F,  0.097545161008064387719862509129598038271069526672363281F, -0.415734806151272562324550108314724639058113098144531250F,
	0.353553390593273786368655464684707112610340118408203125F, -0.353553390593273730857504233426880091428756713867187500F, -0.353553390593273841879806695942534133791923522949218750F,  0.353553390593273730857504233426880091428756713867187500F,  0.353553390593273841879806695942534133791923522949218750F, -0.353553390593273342279445614622090943157672882080078125F, -0.353553390593273619835201770911226049065589904785156250F,  0.353553390593273286768294383364263921976089477539062500F,
	0.277785116509801144335511935423710383474826812744140625F, -0.490392640201615215289621119154617190361022949218750000F,  0.097545161008064137919681968469376442953944206237792969F,  0.415734806151272784369155033346032723784446716308593750F, -0.415734806151272562324550108314724639058113098144531250F, -0.097545161008064013019591698139265645295381546020507812F,  0.490392640201615326311923581670271232724189758300781250F, -0.277785116509800755757453316618921235203742980957031250F,
	0.191341716182544918645191955874906852841377258300781250F, -0.461939766255643369241568052530055865645408630371093750F,  0.461939766255643258219265590014401823282241821289062500F, -0.191341716182544918645191955874906852841377258300781250F, -0.191341716182545279467674959050782490521669387817382812F,  0.461939766255643369241568052530055865645408630371093750F, -0.461939766255643147196963127498747780919075012207031250F,  0.191341716182544779867313877730339299887418746948242188F,
	0.097545161008064151797469776283833198249340057373046875F, -0.277785116509801088824360704165883362293243408203125000F,  0.415734806151272784369155033346032723784446716308593750F, -0.490392640201615326311923581670271232724189758300781250F,  0.490392640201615215289621119154617190361022949218750000F, -0.415734806151272506813398877056897617876529693603515625F,  0.277785116509800755757453316618921235203742980957031250F, -0.097545161008064276697560046613943995907902717590332031F
};


inline size_t shrRoundUp(int group_size, size_t global_size) {
	int r = global_size % group_size;
	if (r == 0)
		return global_size;
	else
		return global_size + group_size - r;
}

//inline void getSingleContextAndDevice(cl_context &theContext, cl_device_id* &theDevices) {
//	cl_int errcode;
//	theContext = clCreateContextFromType(0, CL_DEVICE_TYPE_GPU, NULL, NULL, &errcode);
//	oclAssert(errcode);
//	
//	// get the list of GPU devices associated with context
//	size_t dataBytes;
//	oclAssert( clGetContextInfo(theContext, CL_CONTEXT_DEVICES, 0, NULL, &dataBytes) );
//	theDevices = (cl_device_id *) malloc(dataBytes);
//	oclAssert( clGetContextInfo(theContext, CL_CONTEXT_DEVICES, dataBytes, theDevices, NULL) );
//}

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
		fputs("Couldn't open OpenCL source file\n", stderr);
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
		printf("file %s, line %i\n\n" , __FILE__ , __LINE__);
		oclLogBuildInfo(prog, oclGetFirstDev(theContext));
		oclLogPtx(prog, oclGetFirstDev(theContext), "temporaryDebug.ptx");
		return 0;
	}
	return prog;
}

void computeOnGPU(const char* kernelName, float* &h_idata, size_t size_x, size_t size_y) {
	// Setup Context
	cl_int errcode = CL_SUCCESS;
	cl_context clGPUContext;
	cl_device_id* devIDs;
	
	const cl_uint GPU_COUNT = getContextAndDevices(clGPUContext, devIDs);
	if (GPU_COUNT == 0) {
		return;
	}
	
	// Create a command-queue
	cl_command_queue commandQueue = clCreateCommandQueue(clGPUContext, devIDs[0], CL_QUEUE_PROFILING_ENABLE, &errcode); // CL_QUEUE_PROFILING_ENABLE
	oclAssert(errcode);
	
	free(devIDs);
	
	// Create Program / Kernel
	cl_program clProgram = loadProgram("../jpegenc/opencl/arai.cl", clGPUContext);
	
	
	// Setup Memory
	cl_mem d_idata;
	cl_mem d_odata;
	cl_mem matrix_a;
	cl_kernel clKernel;
	
	size_t sizePerGPU = shrRoundUp(BLOCK_DIM, size_x);
	size_t offset = 0;
	
	const size_t mem_size_in = sizeof(float) * size_x * size_y;
	const size_t mem_size_out = sizeof(float) * size_y * sizePerGPU;
	const size_t mem_size_matrix_a = sizeof(float) * 64;
	
	// Setup device memory
	d_idata = clCreateBuffer(clGPUContext, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, mem_size_in, h_idata, &errcode);
	oclAssert(errcode);
	d_odata = clCreateBuffer(clGPUContext, CL_MEM_WRITE_ONLY, mem_size_out, NULL, &errcode);
	oclAssert(errcode);
	matrix_a = clCreateBuffer(clGPUContext, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, mem_size_matrix_a, (float*)oclMatrixA, &errcode);
	oclAssert(errcode);
	
	// Create kernel
	clKernel = clCreateKernel(clProgram, kernelName, &errcode);
	oclAssert(errcode);
	
	// Launch OpenCL kernel
	errcode  = clSetKernelArg(clKernel, 0, sizeof(cl_mem), (void *) &d_odata);
	errcode |= clSetKernelArg(clKernel, 1, sizeof(cl_mem), (void *) &d_idata);
	errcode |= clSetKernelArg(clKernel, 2, sizeof(cl_mem), (void *) &matrix_a);
	errcode |= clSetKernelArg(clKernel, 3, sizeof(size_t), &offset);
	errcode |= clSetKernelArg(clKernel, 4, sizeof(size_t), &size_x);
	errcode |= clSetKernelArg(clKernel, 5, sizeof(size_t), &size_y);
	errcode |= clSetKernelArg(clKernel, 6, (BLOCK_DIM + 1) * BLOCK_DIM * sizeof(float), 0 );
	errcode |= clSetKernelArg(clKernel, 7, (BLOCK_DIM + 1) * BLOCK_DIM * sizeof(float), 0 );
	oclAssert(errcode);
	
	
	// set up execution configuration
	size_t localWorkSize[2], globalWorkSize[2];
	localWorkSize[0] = BLOCK_DIM;
	localWorkSize[1] = BLOCK_DIM;
	globalWorkSize[0] = sizePerGPU;
	globalWorkSize[1] = shrRoundUp(BLOCK_DIM, size_y);
	
	DebugLog("\nProcessing a %d by %d matrix of floats...\n\n", size_x, size_y);
	oclAssert(clEnqueueNDRangeKernel(commandQueue, clKernel, 2, NULL, globalWorkSize, localWorkSize, 0, NULL, NULL));
	
	// Block CPU till GPU is done
	oclAssert(clFinish(commandQueue));
	
	// Retrieve result from device
	
	size_t size = MIN(size_x - offset, sizePerGPU) * size_y * sizeof(float);
	oclAssert(clEnqueueReadBuffer(commandQueue, d_odata, CL_TRUE, 0, size, &h_idata[offset * size_y], 0, NULL, NULL));
	
	// Cleanup Open CL
	clReleaseContext(clGPUContext);
	clReleaseProgram(clProgram);
	
	errcode |= clReleaseMemObject(d_idata);
	errcode |= clReleaseMemObject(d_odata);
	errcode |= clReleaseMemObject(matrix_a);
	errcode |= clReleaseKernel(clKernel);
	errcode |= clReleaseCommandQueue(commandQueue);
	oclAssert(errcode);
}

void OCL_DCT::printDevices() {
	cl_context context;
	cl_device_id* devIDs;
	
	const cl_uint gpu_count = getContextAndDevices(context, devIDs);
	
	// print all devices
	printf("Devices:\n");
	char device_string[1024];
	for (int i = 0; i < gpu_count; i++) {
		clGetDeviceInfo(devIDs[i], CL_DEVICE_NAME, sizeof(device_string), &device_string, NULL);
		printf(" [%d]: %s\n", i, device_string);
	}
	clReleaseContext(context);
	free(devIDs);
	printf("Using Device [0]\n\n");
}

void OCL_DCT::separated(float* &matrix, size_t width, size_t height) {
	computeOnGPU("dct_separated", matrix, width, height);
}

void OCL_DCT::arai(float* &matrix, size_t width, size_t height) {
	computeOnGPU("dct_arai", matrix, width, height);
}
