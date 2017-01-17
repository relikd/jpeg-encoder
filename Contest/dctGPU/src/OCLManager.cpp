#include "OCLManager.hpp"
#include "oclAssert.h"
#include "oclHelper.h"
#include <cstring>

namespace GPU_SETTINGS {
	static int preferedGPU = -1;
	static cl_platform_id preferedPlatform   = nullptr;
	static cl_device_id   preferedDevice     = nullptr;
	static cl_device_type preferedDeviceType = CL_DEVICE_TYPE_GPU;
}

void OCLManager::setPreferedGPU(int gpu) { GPU_SETTINGS::preferedGPU = gpu; }

void CL_CALLBACK contextCallback(const char* errinfo, const void* /*private_info*/, size_t /*cb*/, void* /*user_data*/) {
	printf("%s\n", errinfo);
}

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

cl_uint getContextAndDevices(cl_context* theContext,
							 cl_device_id** theDevices,
							 cl_platform_id platform   = nullptr,
							 cl_device_type deviceType = CL_DEVICE_TYPE_GPU)
{
	cl_int error;
	if (!platform) {
		if (findNvidiaPlatform(&platform) != CL_SUCCESS) {
			return 0; // no platform found
		}
	}
	
	//Get the devices
	cl_uint uiNumDevices;
	error = clGetDeviceIDs(platform, deviceType, 0, NULL, &uiNumDevices);
	
	if (error != CL_SUCCESS || uiNumDevices == 0) {
		return 0;
	}
	
	cl_device_id* devList = (cl_device_id *) malloc( uiNumDevices * sizeof(cl_device_id) );
	oclAssert( clGetDeviceIDs(platform, deviceType, uiNumDevices, devList, NULL) );
	
	//Create the context
	*theContext = clCreateContext(0, uiNumDevices, devList, contextCallback, NULL, &error);
	oclAssert(error);
	
	*theDevices = devList;
	return uiNumDevices;
}

//  ---------------------------------------------------------------
// |
// |  Get device with highest FLOPS (compute unit * clock frequency)
// |
//  ---------------------------------------------------------------

cl_device_id getMaxFlopsDevice(cl_device_id* list, cl_uint count) {
	unsigned int maxFlops = 0;
	cl_device_id fastestDevice = nullptr;
	
	cl_uint i = count;
	while (i--) {
		cl_uint compute_units, clock_frequency;
		cl_uint currentFlops = oclHelper::flopsForDevice(list[i], &compute_units, &clock_frequency);
		if (currentFlops > maxFlops) {
			maxFlops = currentFlops;
			fastestDevice = list[i];
		}
	}
	return fastestDevice;
}

// ################################################################
// #
// #  Class methods
// #
// ################################################################

cl_uint getPreferedDevice(cl_device_id* device, cl_device_id** list, cl_context* context) {
	// Create context and get device list
	cl_uint n = getContextAndDevices(context, list, GPU_SETTINGS::preferedPlatform, GPU_SETTINGS::preferedDeviceType);
	if (n == 0) return 0;
	
	// select specific device
	if (nullptr != GPU_SETTINGS::preferedDevice)
		*device = GPU_SETTINGS::preferedDevice;
	else if (GPU_SETTINGS::preferedGPU >= 0 && GPU_SETTINGS::preferedGPU < (int)n)
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
	program = oclHelper::loadProgram(path, context);
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
	for (cl_uint i = 0; i < n; i++) {
		oclHelper::printDevice(deviceList[i], i, deviceList[i] == device);
	}
	free(deviceList);
	printf("\n");
	return n;
}

void OCLManager::askUserToSelectGPU() {
	CLPlatformDevice* pdList;
	cl_uint n = oclHelper::getListOfPlattformsAndDevices(&pdList, true);
	if (n == 0)
		return;
	
	unsigned int gpu;
	do {
		printf("Select Device: ");
#ifdef _WIN32
		scanf_s("%i", &gpu, 2);
#else
		scanf("%i", &gpu);
#endif
	} while (gpu >= n);
	
	printf("\n");
	
	CLPlatformDevice sel = pdList[gpu];
	GPU_SETTINGS::preferedPlatform   = sel.platform;
	GPU_SETTINGS::preferedDevice     = sel.device;
	// Because device type can be a combination of types
	GPU_SETTINGS::preferedDeviceType = (sel.type & CL_DEVICE_TYPE_ACCELERATOR ? CL_DEVICE_TYPE_ACCELERATOR
										: sel.type & CL_DEVICE_TYPE_GPU ? CL_DEVICE_TYPE_GPU
										: sel.type & CL_DEVICE_TYPE_CPU ? CL_DEVICE_TYPE_CPU
										: CL_DEVICE_TYPE_DEFAULT );
	free(pdList);
}

