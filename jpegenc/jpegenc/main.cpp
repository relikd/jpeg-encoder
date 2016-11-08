#include <iostream>
#include "io/PPMLoader.hpp"
#include <stdlib.h>

int main(int argc, const char *argv[]) {

	PPMLoader loader;
	clock_t t;
	t = clock();

	auto image = loader.load("/home/marv/Projects/jpeg-encoder/images/very_small.ppm");

//	image->print();

//	RGBToYCbCrConverter converter1;
//	image = converter1.convert(image);

//	image->print();

//	image->channel2->reduceBySubSampling( image->imageSize.width, image->imageSize.height );
//	image->channel3->reduceBySubSampling( image->imageSize.width, image->imageSize.height );

//	YCbCrToRGBConverter converter2;
//	image = converter2.convert(image);
//
//	loader.write("/home/marv/Projects/PixelPal/example_images/output.test.ppm", image);

//	auto image = loader.load("data/output.test.ppm");

//	t = clock() - t;
//	printf ("It took me %lu clicks (%f seconds).\n",t,((float)t)/CLOCKS_PER_SEC);

//	image->print();

	return 0;
}