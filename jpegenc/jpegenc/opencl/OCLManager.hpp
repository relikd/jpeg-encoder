#ifndef OCLManager_hpp
#define OCLManager_hpp

#if defined (__APPLE__) || defined(MACOSX)
#include <OpenCL/opencl.h>
#else
#include <CL/opencl.h>
#endif

#include <stdlib.h>

class OCLManager {
	cl_uint deviceCount;
	cl_device_id* deviceList;
	
public:
	cl_context context;
	cl_device_id device;
	cl_program program;
	cl_command_queue commandQueue;
	
	OCLManager(const char *clFile);
	
	~OCLManager() {
		free(deviceList);
		clReleaseContext(context);
		clReleaseProgram(program);
		clReleaseCommandQueue(commandQueue);
	};
	
	static void printDevices();
	
	static void setPreferedGPU(int gpu);
	static void forceNvidiaPlatform(bool f);
};

#endif /* OCLManager_hpp */
