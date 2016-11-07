#include <iostream>
#include "io/PPMLoader.hpp"
#include "converter/RGBToYCbCrConverter.hpp"
#include "converter/YCbCrToRGBConverter.h"

#include <time.h>

int main(int argc, const char *argv[]) {

	PPMLoader loader;
	clock_t t;
	t = clock();

	auto image = loader.customLoad("/home/marv/Projects/jpeg-encoder/jpegenc/jpegenc/data/singapore4k.test.ppm");

	std::cout << (size_t) image->channel1->getValue( 0, image->imageSize.height - 1, image->imageSize ) << std::endl;

	RGBToYCbCrConverter converter1;
	image = converter1.convert(image);

	std::cout << (size_t) image->channel1->getValue( 0, image->imageSize.height - 1, image->imageSize ) << std::endl;

	image->channel2->reduceBySubSampling( image->imageSize.width, image->imageSize.height );
	image->channel3->reduceBySubSampling( image->imageSize.width, image->imageSize.height );

	std::cout << (size_t) image->channel1->getValue( 0, image->imageSize.height - 1, image->imageSize ) << std::endl;

	YCbCrToRGBConverter converter2;
	image = converter2.convert(image);

	std::cout << (size_t) image->channel1->getValue( 0, image->imageSize.height - 1, image->imageSize ) << std::endl;

	loader.write("/home/marv/Projects/jpeg-encoder/jpegenc/jpegenc/data/output.test.ppm", image);

//	auto image = loader.load("data/singapore4k.test.ppm");
//	image->print();


	t = clock() - t;
	printf ("It took me %lu clicks (%f seconds).\n",t,((float)t)/CLOCKS_PER_SEC);

//	image->print();


	return 0;
}
