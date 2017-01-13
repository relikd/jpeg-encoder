cmake_minimum_required(VERSION 3.6)
project(jpeg_encoder)

file( GLOB_RECURSE SOURCE_FILES jpegenc/jpegenc/* )

# Add C++ 11 support
set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11" )
set( EXECUTABLE_OUTPUT_PATH "../bin" CACHE PATH "Output directory" )

# Open CL
include( FindOpenCL.cmake )
find_package( OpenCL REQUIRED )
include_directories( ${OPENCL_INCLUDE_DIR} )

add_executable( jpeg_encoder ${SOURCE_FILES})
target_link_libraries( jpeg_encoder ${OPENCL_LIBRARIES} )
