#include "OCLManager.hpp"
#include "oclAssert.h"
#include <cstring>

namespace GPU_SETTINGS {
	static int preferedGPU = -1;
	static bool forceNvidiaPlatform = true;
	static cl_device_type deviceType = CL_DEVICE_TYPE_ALL;
}

void OCLManager::setPreferedGPU(int gpu) { GPU_SETTINGS::preferedGPU = gpu; }


void CL_CALLBACK contextCallback(const char* errinfo, const void* /*private_info*/, size_t /*cb*/, void* /*user_data*/) {
	printf("%s\n", errinfo);
}

#pragma mark - Compile Program

// ################################################################
// #
// #  Compile .cl Program
// #
// ################################################################

char* loadFileContent(const char* path, size_t* fileSize) {
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

cl_program loadProgram(const char *path, cl_context &theContext) {
	// Program setup
	size_t program_length;
	char *source = loadFileContent(path, &program_length);
	if (source == NULL) {
		printf("Couldn't open OpenCL source file '%s'\n", path);
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

#pragma mark - Find Platform and Device

// ################################################################
// #
// #  Find Platform and Device
// #
// ################################################################

/**
 * Go through all available platforms and select the one named NVIDIA
 * @param platform Reference to an empty variable
 * @return returns CL_INVALID_PLATFORM if no OpenCL is found
 */
cl_int findNvidiaPlatform(cl_platform_id* platform) {
	// Get OpenCL platform count
	cl_uint platformsCount;
	oclAssert( clGetPlatformIDs(0, NULL, &platformsCount) );
	if (platformsCount == 0) {
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

cl_uint getContextAndDevices(cl_context* theContext, cl_device_id** theDevices) {
	cl_int error;
	cl_platform_id cpPlatform;
	error = findNvidiaPlatform(&cpPlatform);
	
	if (error != CL_SUCCESS) { // no platform found
		return 0;
	}
	
	//Get the devices
	cl_uint uiNumDevices;
	error = clGetDeviceIDs(cpPlatform, CL_DEVICE_TYPE_GPU, 0, NULL, &uiNumDevices);
	
	if (error != CL_SUCCESS || uiNumDevices == 0) {
		return 0;
	}
	
	cl_device_id* devList = (cl_device_id *) malloc( uiNumDevices * sizeof(cl_device_id) );
	oclAssert( clGetDeviceIDs(cpPlatform, CL_DEVICE_TYPE_GPU, uiNumDevices, devList, NULL) );
	
	//Create the context
	*theContext = clCreateContext(0, uiNumDevices, devList, contextCallback, NULL, &error);
	oclAssert(error);
	
	*theDevices = devList;
	return uiNumDevices;
}

cl_uint getDevicesList(cl_context* context, cl_device_id** list) {
	cl_int errcode;
	*context = clCreateContextFromType(0, GPU_SETTINGS::deviceType, contextCallback, NULL, &errcode);
	if (errcode != CL_SUCCESS) {
		return 0;
	}
	
	size_t dataBytes;
	clGetContextInfo(*context, CL_CONTEXT_DEVICES, 0, NULL, &dataBytes);
	
	if (dataBytes == 0) {
		clReleaseContext(*context);
		return 0;
	}
	*list = (cl_device_id*) malloc(dataBytes);
	clGetContextInfo(*context, CL_CONTEXT_DEVICES, dataBytes, *list, NULL);
	
	return (cl_uint)(dataBytes / sizeof(cl_device_id));
}

//  ---------------------------------------------------------------
// |
// |  Get device with highest FLOPS (compute unit * clock frequency)
// |
//  ---------------------------------------------------------------

inline unsigned int flopsForDevice(cl_device_id dev, cl_uint* compute_units, cl_uint* clock_frequency) {
	clGetDeviceInfo(dev, CL_DEVICE_MAX_COMPUTE_UNITS, sizeof(cl_uint), compute_units, NULL);
	clGetDeviceInfo(dev, CL_DEVICE_MAX_CLOCK_FREQUENCY, sizeof(cl_uint), clock_frequency, NULL);
	return (*compute_units) * (*clock_frequency);
}

cl_device_id getMaxFlopsDevice(cl_device_id* list, cl_uint count) {
	unsigned int maxFlops = 0;
	cl_device_id fastestDevice = nullptr;
	
	cl_uint i = count;
	while (i--) {
		cl_uint compute_units, clock_frequency;
		unsigned int currentFlops = flopsForDevice(list[i], &compute_units, &clock_frequency);
		if (currentFlops > maxFlops) {
			maxFlops = currentFlops;
			fastestDevice = list[i];
		}
	}
	return fastestDevice;
}

#pragma mark - Class methods

// ################################################################
// #
// #  Class methods
// #
// ################################################################

cl_uint getPreferedDevice(cl_device_id* device, cl_device_id** list, cl_context* context) {
	cl_uint n;
	// Create context and get device list
	if (GPU_SETTINGS::forceNvidiaPlatform)
		n = getContextAndDevices(context, list);
	else
		n = getDevicesList(context, list);
	
	if (n == 0) return 0;
	
	// select specific device
	if (GPU_SETTINGS::preferedGPU >= 0 && GPU_SETTINGS::preferedGPU < (int)n)
		*device = (*list)[GPU_SETTINGS::preferedGPU];
	else
		*device = getMaxFlopsDevice(*list, n);
	
	return n;
}

OCLManager::OCLManager(const char *path) {
	deviceCount = getPreferedDevice(&device, &deviceList, &context);
	if (deviceCount == 0) {
		managerIsValid = false;
		return;
	}
	
	// Compile program
	program = loadProgram(path, context);
	if (program == 0) {
		// write out the build log
		size_t len = 0;
		clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, 0, NULL, &len );
		char* log = new char[len];
		clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, len * sizeof(char), log, NULL );
		printf("\n-------------------------------------\nBuild Log:\n%s\n-------------------------------------\n", log);
		delete [] log;
		exit(EXIT_FAILURE);
	}
	// Create a command-queue
	cl_int errcode = CL_SUCCESS;
	commandQueue = clCreateCommandQueue(context, device, CL_QUEUE_PROFILING_ENABLE, &errcode);
	oclAssert(errcode);
}

bool OCLManager::hasValidDevice() {
	cl_context context;
	cl_device_id device;
	cl_device_id* deviceList;
	cl_uint n = getPreferedDevice(&device, &deviceList, &context);
	if (n == 0) {
		return false;
	}
	clReleaseContext(context);
	free(deviceList);
	return true;
}

cl_uint OCLManager::printDevices() {
	cl_context context;
	cl_device_id device;
	cl_device_id* deviceList;
	cl_uint n = getPreferedDevice(&device, &deviceList, &context);
	if (n == 0) {
		fputs("No OpenCL device available\n", stderr);
		return 0;
	}
	clReleaseContext(context);
	
	// Print devices
	printf("Devices:\n");
	char device_string[1024];
	for (cl_uint i = 0; i < n; i++) {
		cl_uint compute_units, clock_frequency;
		cl_ulong gpu_mem_size;
		
		flopsForDevice(deviceList[i], &compute_units, &clock_frequency);
		clGetDeviceInfo(deviceList[i], CL_DEVICE_NAME, sizeof(device_string), &device_string, NULL);
		clGetDeviceInfo(deviceList[i], CL_DEVICE_GLOBAL_MEM_SIZE, sizeof(cl_ulong), &gpu_mem_size, NULL);
		
		if (deviceList[i] == device) {
			printf("-> "); // mark selected device
		} else {
			printf("   ");
		}
		printf("[%d]: %s (%d compute units, %llumb, %dMhz)\n",
			   i, device_string, compute_units, (gpu_mem_size/1024/1024), clock_frequency);
	}
	free(deviceList);
	printf("\n");
	return n;
}

void OCLManager::askUserToSelectGPU() {
	GPU_SETTINGS::forceNvidiaPlatform = false;
	
	if (printDevices() == 0) {
		return;
	}
	
	unsigned char gpu;
	printf("Select Device: ");
#ifdef _WIN32
	scanf_s("%c", &gpu, 2);
#else
	scanf("%c", &gpu);
#endif
	
	GPU_SETTINGS::preferedGPU = (gpu - 48); // char to int
	
	cl_context context;
	cl_device_id correctDevice;
	cl_device_id* deviceList;
	cl_uint tmp = getPreferedDevice(&correctDevice, &deviceList, &context);
	if (tmp == 0)
		return;
	
	free(deviceList);
	clReleaseContext(context);
	
	cl_device_type type;
	clGetDeviceInfo(correctDevice, CL_DEVICE_TYPE, sizeof(cl_device_type), &type, NULL);
	
	// avoid devices with multiple types
	if (type & CL_DEVICE_TYPE_CUSTOM)            GPU_SETTINGS::deviceType = CL_DEVICE_TYPE_CUSTOM;
	else if (type & CL_DEVICE_TYPE_ACCELERATOR)  GPU_SETTINGS::deviceType = CL_DEVICE_TYPE_ACCELERATOR;
	else if (type & CL_DEVICE_TYPE_GPU)          GPU_SETTINGS::deviceType = CL_DEVICE_TYPE_GPU;
	else if (type & CL_DEVICE_TYPE_CPU)          GPU_SETTINGS::deviceType = CL_DEVICE_TYPE_CPU;
	else                                         GPU_SETTINGS::deviceType = CL_DEVICE_TYPE_DEFAULT;
	
	// Retrive the context a second time, to get the device-type specific list (and index)
	GPU_SETTINGS::preferedGPU = 0;
	cl_device_id wrongDevice;
	cl_uint n = getPreferedDevice(&wrongDevice, &deviceList, &context);
	while (n--) {
		if (deviceList[n] == correctDevice) {
			GPU_SETTINGS::preferedGPU = n;
		}
	}
}

