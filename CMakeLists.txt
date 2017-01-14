cmake_minimum_required(VERSION 3.6)
project(jpeg_encoder)

file( GLOB_RECURSE SOURCE_FILES jpegenc/jpegenc/* )

# Add C++ 11 support
set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11" )
set( EXECUTABLE_OUTPUT_PATH "../bin" CACHE PATH "Output directory" )

add_executable( jpeg_encoder ${SOURCE_FILES})
