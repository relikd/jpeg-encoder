#include <iostream>
#include "io/PPMLoader.hpp"
#include "bitstream/BitstreamMarcel.hpp"
#include <stdlib.h>

int main(int argc, const char *argv[]) {

	PPMLoader loader;
	clock_t t;
	t = clock();
	
//	auto image = loader.load("data/very_small.ppm");
//	auto image = loader.load("data/singapore4k.test.ppm");
//	auto image = loader.load("data/gigantic.test.ppm");

//	image->print();

//	RGBToYCbCrConverter converter1;
//	image = converter1.convert(image);

//	image->print();

//	image->channel2->reduceBySubSampling( image->imageSize.width, image->imageSize.height );
//	image->channel3->reduceBySubSampling( image->imageSize.width, image->imageSize.height );

//	YCbCrToRGBConverter converter2;
//	image = converter2.convert(image);
//
//	loader.write("data/output.test.ppm", image);

//	auto image = loader.load("data/output.test.ppm");

	t = clock() - t;
	printf ("It took me %lu clicks (%f seconds).\n",t,((float)t)/CLOCKS_PER_SEC);

//	image->print();

//	BitstreamMarcel bitstream;
//
//	for (int i = 0; i < 16; ++i) {
//		bitstream.add(true);
//	}
//
//	bitstream.print();

	//cout << bitstream.read(30) << endl;

	return 0;
}
