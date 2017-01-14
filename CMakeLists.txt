cmake_minimum_required(VERSION 3.6)
project(jpeg_encoder)

file( GLOB_RECURSE SOURCE_FILES jpegenc/jpegenc/* )

# Add C++ 11 support
set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11" )
set( EXECUTABLE_OUTPUT_PATH "_bin" CACHE PATH "Output directory" )

add_executable( jpeg_encoder ${SOURCE_FILES})

# Open CL
IF (APPLE)
	FIND_PATH(OPENCL_INCLUDE_DIR OpenCL/cl.h DOC "Include for OpenCL on OSX")
	FIND_LIBRARY(OPENCL_LIBRARIES OpenCL DOC "OpenCL lib for OSX")
	target_link_libraries( jpeg_encoder ${OPENCL_LIBRARIES} )
ELSE (APPLE)
	## C/C++ -> General -> Additional Include Directories
	#   $(CUDA_PATH)\include
	#   $(AMDAPPSDKROOT)\include
	#   $(INTELOCLSDKROOT)\include
	# can be added with 'include_directories(...)'
	# but I couldnt figure out how to prevent working dir

	## Linker-> General -> Additional Dependencies
	#   $(CUDA_PATH)\lib\x64     || or Win32
	#   $(AMDAPPSDKROOT)\lib\x86_64  || or x86
	#   $(INTELOCLSDKROOT)\lib\x64   || or x86
	target_link_libraries( jpeg_encoder "OpenCL.lib" )
ENDIF (APPLE)
