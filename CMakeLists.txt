cmake_minimum_required(VERSION 3.6)
project(jpeg_encoder)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES
        jpegenc/jpegenc/model/Image.cpp
        jpegenc/jpegenc/model/Image.hpp
    jpegenc/jpegenc/main.cpp
        jpegenc/jpegenc/loader/PPMLoader.cpp
        jpegenc/jpegenc/loader/PPMLoader.hpp
    LICENSE jpegenc/jpegenc/converter/RGBToYCbCrConverter.cpp jpegenc/jpegenc/converter/RGBToYCbCrConverter.hpp)

add_executable(jpeg_encoder ${SOURCE_FILES})