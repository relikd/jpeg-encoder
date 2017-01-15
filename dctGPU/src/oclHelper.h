#ifndef oclHelper_hpp
#define oclHelper_hpp

#if defined (__APPLE__) || defined(MACOSX)
#include <OpenCL/opencl.h>
#else
#include <CL/opencl.h>
#endif

//#include <stdlib.h>
struct CLPlatformDevice {
	cl_platform_id platform;
	cl_device_id device;
	cl_device_type type;
	
	CLPlatformDevice(cl_platform_id p, cl_device_id d, cl_device_type t) : platform(p), device(d), type(t) {};
};

class oclHelper {
public:
	static cl_uint getListOfPlattformsAndDevices(CLPlatformDevice** list, bool print = false); // remember to free
	
	static cl_program loadProgram(const char *path, cl_context &theContext);
	
	static cl_uint flopsForDevice(cl_device_id dev, cl_uint* compute_units, cl_uint* clock_frequency);
	static void printDevice(cl_device_id device, cl_uint index, bool selected = false);
};


#endif
