cmake_minimum_required(VERSION 3.6)
project(dctGPU)

file( GLOB_RECURSE SOURCE_FILES src/* )
file( GLOB CL_FILES opencl/* )

# Add C++ 11 support
set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11" )
set( EXECUTABLE_OUTPUT_PATH "../bin" CACHE PATH "Output directory" )

add_executable( dctGPU ${SOURCE_FILES} ${CL_FILES} )
set_property( DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT dctGPU )

# copy all .cl files automatically to bin
add_custom_command(
	TARGET dctGPU
	PRE_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_directory
		${CMAKE_SOURCE_DIR}/opencl $<TARGET_FILE_DIR:dctGPU>
)

# Open CL
IF (APPLE)
	FIND_PATH(OPENCL_INCLUDE_DIR OpenCL/cl.h DOC "Include for OpenCL on OSX")
	FIND_LIBRARY(OPENCL_LIBRARIES OpenCL DOC "OpenCL lib for OSX")
	target_link_libraries( dctGPU ${OPENCL_LIBRARIES} )
ELSE (APPLE)
	SET(OPENCL_LIBRARY "INTEL_OCL_SDK" CACHE STRING "library selected at CMake configure time")
	SET_PROPERTY(CACHE OPENCL_LIBRARY PROPERTY STRINGS AMD_APP_SDK INTEL_OCL_SDK NVIDIA_CUDA) 
	
	IF(${OPENCL_LIBRARY} MATCHES "AMD_APP_SDK")
		message("Using OpenCL SDK: AMD App SDK")
		include_directories($ENV{AMDAPPSDKROOT}/include)
		target_link_libraries( dctGPU $ENV{AMDAPPSDKROOT}/lib/x86_64/OpenCL.lib ) # or x86
	ENDIF()
	IF(${OPENCL_LIBRARY} MATCHES "INTEL_OCL_SDK")
		message("Using OpenCL SDK: Intel OCL SDK")
		include_directories($ENV{INTELOCLSDKROOT}/include)
		target_link_libraries( dctGPU $ENV{INTELOCLSDKROOT}/lib/x64/OpenCL.lib ) # or x86
	ENDIF()
	IF(${OPENCL_LIBRARY} MATCHES "NVIDIA_CUDA")
		message("Using OpenCL SDK: Nvidia CUDA")
		include_directories($ENV{CUDA_PATH}/include)
		target_link_libraries( dctGPU $ENV{CUDA_PATH}/lib/x64/OpenCL.lib ) # or Win32
	ENDIF()
ENDIF (APPLE)

