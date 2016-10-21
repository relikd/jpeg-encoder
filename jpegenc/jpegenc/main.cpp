#include <iostream>
#include "loader/PPMLoader.hpp"
#include "converter/RGBToYCbCrConverter.hpp"
#include "converter/YCbCrToRGBConverter.h"

int main(int argc, const char *argv[]) {

	PPMLoader loader;
	auto image = loader.load("/home/marv/Projects/jpeg-encoder/jpegenc/jpegenc/data/very_small.ppm");
	image->print();

//	RGBToYCbCrConverter converter1;
//	image = converter1.convert(image);

//	YCbCrToRGBConverter converter2;
//	image = converter2.convert(image);

//	image->reduceBySubSamplingChannel1(2);
//	image->reduceBySubSamplingChannel2(2);
//	image->reduceBySubSamplingChannel3(2);

	image->reduceByAveragingChannel1(2);
	image->reduceByAveragingChannel2(2);
	image->reduceByAveragingChannel3(2);

	image->print();

	return 0;
}