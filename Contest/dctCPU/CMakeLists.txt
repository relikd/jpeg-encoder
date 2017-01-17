cmake_minimum_required(VERSION 3.6)
project(dctCPU)

file( GLOB_RECURSE SOURCE_FILES src/* )

# Add C++ 11 support
set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11" )
set( EXECUTABLE_OUTPUT_PATH "../bin" CACHE PATH "Output directory" )

add_executable( dctCPU ${SOURCE_FILES} )
