cmake_minimum_required(VERSION 3.6)
project(jpeg_encoder)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES
    jpegenc/jpegenc/Image.cpp
    jpegenc/jpegenc/Image.hpp
    jpegenc/jpegenc/main.cpp
    jpegenc/jpegenc/ParserYCbCr.cpp
    jpegenc/jpegenc/ParserYCbCr.hpp
    jpegenc/jpegenc/PPMLoader.cpp
    jpegenc/jpegenc/PPMLoader.hpp
    LICENSE)

add_executable(jpeg_encoder ${SOURCE_FILES})