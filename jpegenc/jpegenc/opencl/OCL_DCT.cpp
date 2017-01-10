#include "OCL_DCT.h"
#include "oclAssert.h"
#include <cstring>

// All OpenCL headers
#if defined (__APPLE__) || defined(MACOSX)
#include <OpenCL/opencl.h>
#else
#include <CL/opencl.h>
#endif

namespace GPU_SETTINGS {
	static int preferedGPU = -1;
	static bool forceNvidiaPlatform = false;
}

void OCL_DCT::setPreferedGPU(int gpu) { GPU_SETTINGS::preferedGPU = gpu; }
void OCL_DCT::forceNvidiaPlatform(bool f) { GPU_SETTINGS::forceNvidiaPlatform = f; }

#define BLOCK_DIM 8 // same block size like cl kernel

static const cl_device_type deviceTypes = CL_DEVICE_TYPE_DEFAULT | CL_DEVICE_TYPE_GPU | CL_DEVICE_TYPE_ACCELERATOR;

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

//  ---------------------------------------------------------------
// |
// |  Helper
// |
//  ---------------------------------------------------------------

int flopsForDevice(cl_device_id dev, cl_uint* compute_units, cl_uint* clock_frequency) {
	clGetDeviceInfo(dev, CL_DEVICE_MAX_COMPUTE_UNITS, sizeof(cl_uint), compute_units, NULL);
	clGetDeviceInfo(dev, CL_DEVICE_MAX_CLOCK_FREQUENCY, sizeof(cl_uint), clock_frequency, NULL);
	return (*compute_units) * (*clock_frequency);
}

#pragma mark - Find Platform and Device

// ################################################################
// #
// #  Find Platform and Device
// #
// ################################################################

inline cl_int findNvidiaPlatform(cl_platform_id* platform) {
	// Get OpenCL platform count
	cl_uint platformsCount;
	oclAssert( clGetPlatformIDs(0, NULL, &platformsCount) );
	if (platformsCount == 0) {
		printf("No OpenCL platform found!\n\n");
		platform = NULL;
		return CL_INVALID_PLATFORM;
	}
	
	// Get platform info
	cl_platform_id* list = (cl_platform_id*)malloc(platformsCount * sizeof(cl_platform_id));
	oclAssert( clGetPlatformIDs(platformsCount, list, NULL) );
	
	// Default to zeroeth platform if NVIDIA not found
	*platform = list[0];
	
	// Find NVIDIA platform if present
	char platformString[1024];
	cl_uint i = platformsCount;
	while (i--) {
		oclAssert( clGetPlatformInfo(list[i], CL_PLATFORM_NAME, sizeof(platformString), &platformString, NULL) );
		if ( strstr(platformString, "NVIDIA") != NULL ) {
			*platform = list[i];
			break;
		}
	}
	free(list);
	
	return CL_SUCCESS;
}

inline cl_uint getContextAndDevices(cl_context* theContext, cl_device_id** theDevices) {
	cl_platform_id cpPlatform;
	oclAssert( findNvidiaPlatform(&cpPlatform) );
	
	//Get the devices
	cl_uint uiNumDevices;
	oclAssert( clGetDeviceIDs(cpPlatform, CL_DEVICE_TYPE_GPU, 0, NULL, &uiNumDevices) );
	
	if (uiNumDevices == 0) {
		printf(" There are no devices supporting OpenCL\n\n");
		return 0;
	}
	
	*theDevices = (cl_device_id *) malloc( uiNumDevices * sizeof(cl_device_id) );
	oclAssert( clGetDeviceIDs(cpPlatform, CL_DEVICE_TYPE_GPU, uiNumDevices, *theDevices, NULL) );
	
	//Create the context
	cl_int errcode;
	*theContext = clCreateContext(0, uiNumDevices, *theDevices, NULL, NULL, &errcode);
	oclAssert(errcode);
	
	return uiNumDevices;
}

//  ---------------------------------------------------------------
// |  Generic, find best device no matter if NVIDIA
//  ---------------------------------------------------------------

cl_device_id getMaxFlopsDevice(cl_context context, int returnInstead = -1) {
	size_t dataBytes;
	clGetContextInfo(context, CL_CONTEXT_DEVICES, 0, NULL, &dataBytes);
	
	cl_device_id* list = (cl_device_id*) malloc(dataBytes);
	clGetContextInfo(context, CL_CONTEXT_DEVICES, dataBytes, list, NULL);
	
	cl_device_id fastestDevice = nullptr;
	int maxFlops = 0;
	
	size_t i = dataBytes / sizeof(cl_device_id);
	while (i--) {
		if (i == returnInstead) { // override automatic selection with manual sel
			fastestDevice = list[i];
			break;
		}
		cl_uint compute_units, clock_frequency;
		int currentFlops = flopsForDevice(list[i], &compute_units, &clock_frequency);
		if (maxFlops < currentFlops) {
			maxFlops = currentFlops;
			fastestDevice = list[i];
		}
	}
	free(list);
	return fastestDevice;
}

size_t getDevicesList(cl_device_id** list, cl_device_id* fastest, int indexInstead = -1) {
	cl_int errcode;
	cl_context context = clCreateContextFromType(0, deviceTypes, NULL, NULL, &errcode);
	oclAssert(errcode);
	
	*fastest = getMaxFlopsDevice(context, indexInstead);
	
	size_t dataBytes;
	clGetContextInfo(context, CL_CONTEXT_DEVICES, 0, NULL, &dataBytes);
	
	*list = (cl_device_id*) malloc(dataBytes);
	clGetContextInfo(context, CL_CONTEXT_DEVICES, dataBytes, *list, NULL);
	
	return dataBytes / sizeof(cl_device_id);
}

#pragma mark - Compile Program

// ################################################################
// #
// #  Compile .cl Program
// #
// ################################################################

inline char* loadFileContent(const char* path, size_t* fileSize) {
	FILE* file = NULL;
	
#ifdef _WIN32
	if (fopen_s(&file, path, "rb") != 0)
		return NULL;
#else
	file = fopen(path, "rb");
	if (file == 0)
		return NULL;
#endif
	
	fseek(file, 0, SEEK_END);
	size_t length = ftell(file);
	fseek(file, 0, SEEK_SET);
	
	// allocate a buffer for the source code string and read it in
	char* buffer = (char *)malloc(length);
	if (fread(buffer, length, 1, file) != 1) {
		fclose(file);
		free(buffer);
		fileSize = 0;
		return 0;
	}
	fclose(file);
	*fileSize = length;
	return buffer;
}

inline cl_program loadProgram(const char *path, cl_context &theContext) {
	// Program setup
	size_t program_length;
	char *source = loadFileContent(path, &program_length);
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
		printf("file %s, line %i\n\n" , __FILE__ , __LINE__);
		return 0;
	}
	return prog;
}

#pragma mark - Actual computation

// ################################################################
// #
// #  Actual computation
// #
// ################################################################

void computeOnGPU(const char* kernelName, float* &h_idata, size_t size_x, size_t size_y, const bool isSeparated) {
	// Setup Context
	cl_int errcode = CL_SUCCESS;
	cl_context clGPUContext;
	cl_device_id selectedDevice;
	
	if (GPU_SETTINGS::forceNvidiaPlatform) {
		cl_device_id* devIDs;
		getContextAndDevices(&clGPUContext, &devIDs);
		selectedDevice = devIDs[GPU_SETTINGS::preferedGPU];
		free(devIDs);
	}
	else // auto detect fastest device
	{
		clGPUContext = clCreateContextFromType(0, deviceTypes, NULL, NULL, &errcode);
		selectedDevice = getMaxFlopsDevice(clGPUContext, GPU_SETTINGS::preferedGPU);
	}
	
	
	// Create a command-queue
	cl_command_queue commandQueue = clCreateCommandQueue(clGPUContext, selectedDevice, CL_QUEUE_PROFILING_ENABLE, &errcode);
	oclAssert(errcode);
	
	// Create Program / Kernel
	cl_program clProgram = loadProgram("../jpegenc/opencl/arai.cl", clGPUContext);
	
	
	// Setup Memory
	cl_mem d_idata;
	cl_mem d_odata;
	cl_mem matrix_a;
	cl_kernel clKernel;
	
	
	const size_t mem_size_in = sizeof(float) * size_x * size_y;
	const size_t mem_size_out = sizeof(float) * size_y * size_x;
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
	if (isSeparated) {
		errcode  = clSetKernelArg(clKernel, 0, sizeof(cl_mem), (void *) &d_odata);
		errcode |= clSetKernelArg(clKernel, 1, sizeof(cl_mem), (void *) &d_idata);
		errcode |= clSetKernelArg(clKernel, 2, sizeof(cl_mem), (void *) &matrix_a);
		errcode |= clSetKernelArg(clKernel, 3, sizeof(size_t), &size_x);
		errcode |= clSetKernelArg(clKernel, 4, sizeof(size_t), &size_y);
		errcode |= clSetKernelArg(clKernel, 5, (BLOCK_DIM + 1) * BLOCK_DIM * sizeof(float), 0 );
		errcode |= clSetKernelArg(clKernel, 6, (BLOCK_DIM + 1) * BLOCK_DIM * sizeof(float), 0 );
	} else {
		errcode  = clSetKernelArg(clKernel, 0, sizeof(cl_mem), (void *) &d_odata);
		errcode |= clSetKernelArg(clKernel, 1, sizeof(cl_mem), (void *) &d_idata);
		errcode |= clSetKernelArg(clKernel, 2, sizeof(size_t), &size_x);
		errcode |= clSetKernelArg(clKernel, 3, sizeof(size_t), &size_y);
		errcode |= clSetKernelArg(clKernel, 4, (BLOCK_DIM + 1) * BLOCK_DIM * sizeof(float), 0 );
	}
	oclAssert(errcode);
	
	
	// set up execution configuration
	size_t localWorkSize[2], globalWorkSize[2];
	localWorkSize[0] = BLOCK_DIM;
	localWorkSize[1] = BLOCK_DIM;
	globalWorkSize[0] = size_x;
	globalWorkSize[1] = size_y;
	
	oclAssert(clEnqueueNDRangeKernel(commandQueue, clKernel, 2, NULL, globalWorkSize, localWorkSize, 0, NULL, NULL));
	
	// Block CPU till GPU is done
	oclAssert(clFinish(commandQueue));
	
	// Retrieve result from device
	
	size_t size = size_x * size_y * sizeof(float);
	oclAssert(clEnqueueReadBuffer(commandQueue, d_odata, CL_TRUE, 0, size, &h_idata[0], 0, NULL, NULL));
	
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

#pragma mark - Static Class Functions

// ################################################################
// #
// #  Static Class Functions
// #
// ################################################################

size_t findPreferredDevice(cl_device_id** allDevices, cl_device_id* selectedDevice) {
	size_t device_count;
	if (GPU_SETTINGS::forceNvidiaPlatform) {
		cl_context context;
		device_count = getContextAndDevices(&context, allDevices);
		clReleaseContext(context);
		
		if (GPU_SETTINGS::preferedGPU < device_count && GPU_SETTINGS::preferedGPU >= 0) {
			*selectedDevice = *allDevices[GPU_SETTINGS::preferedGPU];
		} else {
			printf("There is no GPU device with id: %d\n", GPU_SETTINGS::preferedGPU);
			exit(EXIT_FAILURE);
		}
	}
	else // auto detect fastest device
	{
		device_count = getDevicesList(allDevices, selectedDevice, GPU_SETTINGS::preferedGPU);
	}
	return device_count;
}

void OCL_DCT::printDevices() {
	cl_device_id* devIDs; // dont forget to free
	cl_device_id selectedDevice;
	
	size_t device_count = findPreferredDevice(&devIDs, &selectedDevice);
	
	// print all devices
	printf("Devices:\n");
	char device_string[1024];
	for (int i = 0; i < device_count; i++) {
		cl_uint compute_units, clock_frequency;
		cl_ulong gpu_mem_size;
		
		flopsForDevice(devIDs[i], &compute_units, &clock_frequency);
		clGetDeviceInfo(devIDs[i], CL_DEVICE_NAME, sizeof(device_string), &device_string, NULL);
		clGetDeviceInfo(devIDs[i], CL_DEVICE_GLOBAL_MEM_SIZE, sizeof(cl_ulong), &gpu_mem_size, NULL);
		
		if (devIDs[i] == selectedDevice) {
			printf("-> ");
		} else {
			printf("   ");
		}
		
		printf("[%d]: %s (%d compute units, %llumb, %dMhz)\n",
			   i, device_string, compute_units, (gpu_mem_size/1024/1024), clock_frequency);
	}
	free(devIDs);
	
	printf("\n");
}

void OCL_DCT::separated(float* &matrix, size_t width, size_t height) {
	computeOnGPU("dct_separated", matrix, width, height, true);
}

void OCL_DCT::arai(float* &matrix, size_t width, size_t height) {
	computeOnGPU("dct_arai", matrix, width, height, false);
}
