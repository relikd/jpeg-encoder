#include "oclHelper.h"
#include "oclAssert.h"

cl_uint oclHelper::flopsForDevice(cl_device_id dev, cl_uint* compute_units, cl_uint* clock_frequency) {
	clGetDeviceInfo(dev, CL_DEVICE_MAX_COMPUTE_UNITS, sizeof(cl_uint), compute_units, NULL);
	clGetDeviceInfo(dev, CL_DEVICE_MAX_CLOCK_FREQUENCY, sizeof(cl_uint), clock_frequency, NULL);
	return (*compute_units) * (*clock_frequency);
}

void oclHelper::printDevice(cl_device_id device, cl_uint index, bool selected) {
	char device_string[1024];
	cl_uint compute_units, clock_frequency;
	cl_ulong gpu_mem_size;
	
	flopsForDevice(device, &compute_units, &clock_frequency);
	clGetDeviceInfo(device, CL_DEVICE_NAME, sizeof(device_string), &device_string, NULL);
	clGetDeviceInfo(device, CL_DEVICE_GLOBAL_MEM_SIZE, sizeof(cl_ulong), &gpu_mem_size, NULL);
	
	if(selected) printf("-> ");
	else         printf("   ");
	printf("[%d]: %s (%d compute units, %llumb, %dMhz)\n",
		   index, device_string, compute_units, (gpu_mem_size/1024/1024), clock_frequency);
}

// ################################################################
// #
// #  Get a list of all platforms and all devices for each platform
// #
// ################################################################

cl_uint oclHelper::getListOfPlattformsAndDevices(CLPlatformDevice** list, bool print) {
	cl_uint platformCount;
	oclAssert( clGetPlatformIDs(0, NULL, &platformCount) );
	if (platformCount == 0) {
		return 0;
	}
	
	// Get platform info
	cl_platform_id* pList = (cl_platform_id*)malloc(platformCount * sizeof(cl_platform_id));
	oclAssert( clGetPlatformIDs(platformCount, pList, NULL) );
	
	
	unsigned short pdCounter = 0;
	CLPlatformDevice* allPDs = (CLPlatformDevice*)malloc(sizeof(CLPlatformDevice) * 20);
	
	char platformString[1024];
	cl_uint p = platformCount;
	while (p--) {
		oclAssert( clGetPlatformInfo(pList[p], CL_PLATFORM_NAME, sizeof(platformString), &platformString, NULL) );
		if (print)
			printf("Platform: %s\n", platformString);
		
		//Get the devices
		cl_uint uiNumDevices;
		cl_int error = clGetDeviceIDs(pList[p], CL_DEVICE_TYPE_ALL, 0, NULL, &uiNumDevices);
		
		if (error != CL_SUCCESS || uiNumDevices == 0) {
			if (print)
				printf("No devices\n");
			continue;
		}
		
		cl_device_id* devList = (cl_device_id *) malloc( uiNumDevices * sizeof(cl_device_id) );
		oclAssert( clGetDeviceIDs(pList[p], CL_DEVICE_TYPE_ALL, uiNumDevices, devList, NULL) );
		
		cl_uint d = uiNumDevices;
		while (d--) {
			if (print)
				printDevice(devList[d], pdCounter);
			
			// Get type and append to list
			cl_device_type typ;
			clGetDeviceInfo(devList[d], CL_DEVICE_TYPE, sizeof(cl_device_type), &typ, NULL);
			allPDs[pdCounter] = CLPlatformDevice(pList[p], devList[d], typ);
			pdCounter++;
		}
		free(devList);
	}
	free(pList);
	
	*list = allPDs;
	return pdCounter;
}


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

cl_program oclHelper::loadProgram(const char *path, cl_context &theContext) {
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

