#include <iostream>
#include <stdlib.h>
#include "io/PPMLoader.hpp"
#include "converter/RGBToYCbCrConverter.hpp"
#include "converter/YCbCrToRGBConverter.hpp"
#include "helper/Test.hpp"

#include "bitstream/Bitstream.hpp"

#include "segments/JPEGSegments.hpp"

using namespace JPEGSegments;

#define TEST_ITERATIONS 10000000
#define TEST_REPEAT 10

//  ---------------------------------------------------------------
// |
// |  PPM Image Processing
// |
//  ---------------------------------------------------------------

void testImage() {
	std::cout << "Loading image ..." << std::endl;
	Test::performance([]{
		PPMLoader loader;
		auto image = loader.load("data/singapore4k.test.ppm");
		
//		RGBToYCbCrConverter converter1;
//		converter1.convert(image);
//
//		image->print();
//
//		image->channel2->reduceBySubSampling( image->imageSize.width, image->imageSize.height );
//		image->channel3->reduceBySubSampling( image->imageSize.width, image->imageSize.height );
//
//		YCbCrToRGBConverter converter2;
//		converter2.convert(image);
//
//		image->reduceBySubSample(2, 2);
//		image->reduceByAverage(2, 2);
//		image->print();

//		Test::performance([&loader, &image]{
//			loader.write("data/output.test.ppm", image);
//		});
	});
}

//  ---------------------------------------------------------------
// |
// |  JPEG Writer
// |
//  ---------------------------------------------------------------

void testJPEGWriter() {
    
    Bitstream bitStream;
    bitStream.add(1);
    bitStream.add(0);
    bitStream.add(1);
    bitStream.add(1);
    bitStream.add(0);
    bitStream.print();
    bitStream.fillup(1);
    bitStream.print();
    
    PPMLoader loader;
    auto image = loader.load("data/very_small.ppm");
    
    JPEGWriter writer;
    writer.writeJPEGImage(image, "Test.test.jpg");
}

// ################################################################
// #
// #  Main
// #
// ################################################################

int main(int argc, const char *argv[]) {
	testJPEGWriter();
	
	return 0;
}
