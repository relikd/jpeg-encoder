#include <iostream>
#include "loader/PPMLoader.hpp"
#include "converter/RGBToYCbCrConverter.hpp"

int main(int argc, const char *argv[]) {

	PPMLoader loader;
	auto image = loader.load("/home/marv/Projects/jpeg-encoder/jpegenc/jpegenc/data/very_small.ppm");

	image->print();

	RGBToYCbCrConverter converter1;
	image = converter1.convert( image );

	image->print();
	return 0;
}