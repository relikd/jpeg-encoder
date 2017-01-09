cmake_minimum_required(VERSION 3.6)
project(jpeg_encoder)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

file(GLOB_RECURSE SOURCE_FILES jpegenc/jpegenc/*)

add_executable(jpeg_encoder ${SOURCE_FILES})
