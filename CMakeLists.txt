cmake_minimum_required(VERSION 3.6)
project(jpeg-encoder)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES
        jpegenc/jpegenc/main.cpp
        jpegenc/jpegenc/Image.hpp
        jpegenc/jpegenc/Image.cpp
        jpegenc/jpegenc/PPMLoader.hpp
        jpegenc/jpegenc/PPMLoader.cpp
        )
add_executable(jpeg-encoder ${SOURCE_FILES})