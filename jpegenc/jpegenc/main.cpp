#include <iostream>
#include "loader/PPMLoader.hpp"
#include "converter/RGBToYCbCrConverter.hpp"
#include "converter/YCbCrToRGBConverter.h"

int main(int argc, const char *argv[]) {

	PPMLoader loader;
//	auto image = loader.load("data/very_small.ppm");
	auto image = loader.load("/home/marv/Projects/jpeg-encoder/jpegenc/jpegenc/data/very_small.ppm");
	image->print();

//	RGBToYCbCrConverter converter1;
//	image = converter1.convert(image);

//	YCbCrToRGBConverter converter2;
//	image = converter2.convert(image);

//	image->channel1->reduceBySubSampling(2);
//	image->channel2->reduceBySubSampling(2);
//	image->channel3->reduceBySubSampling(2);

//	image->channel1->reduceByAveraging(2);
//	image->channel2->reduceByAveraging(2);
//	image->channel3->reduceByAveraging(2);

	image->print();

	return 0;
}
