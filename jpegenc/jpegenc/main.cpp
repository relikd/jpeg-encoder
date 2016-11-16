#include <iostream>
#include <stdlib.h>
#include "io/PPMLoader.hpp"
#include "converter/RGBToYCbCrConverter.hpp"
#include "converter/YCbCrToRGBConverter.hpp"
#include "helper/Test.hpp"

#include "bitstream/Bitstream.hpp"

#include "JPEGSegments.hpp"

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
// |  Bitstream
// |
//  ---------------------------------------------------------------

void testChris() {
	StartOfFrame0* sof0 = new StartOfFrame0(18, 18, 18);
	APP0* app0 = new APP0();
	
	std::vector<JpegSegment*> segments;
	
	segments.push_back(sof0);
	segments.push_back(app0);
	
	Bitstream stream;
	for (int i = 0; i < segments.size(); ++i) {
		segments[i]->addToStream(stream);
	}
	
	stream.saveToFile("Test.jpg");
}

void testMarcel() {
}

void testMarv() {
}

void testOleg(bool testSingleBit = false, bool testByteBit = false, bool testRead = false, bool testWriteFile = false) {
	Bitstream bitstream;
//	bitstream.add(true);
//	bitstream.add(false);
//	bitstream.add(false);
//	bitstream.add(true);
	bitstream.add('A', 8); // 0100 0001
	bitstream.add('x', 8); // 0111 1000
	bitstream.add('l', 8); // 0110 1100
	bitstream.add('D', 8); // 0100 0100
	bitstream.add(' ', 8); // 0010 0000
//	for (size_t i = 0; i < 524288*8-2; ++i) {
//		bitstream.add(1);
//	}
	bitstream.add(255, 6);
//	bitstream.add(0);
//	bitstream.saveToFile("data/out.txt");
	bitstream.add(0);
	bitstream.add(1);
	bitstream.add(0);
//	bitstream.saveToFile("data/out2.txt");
//	bitstream.print();
	
	
	std::cout << "Testing, Oleg" << std::endl;
	
	if (testSingleBit) {
		std::cout << "Write single bit: ";
		Test::performance(TEST_ITERATIONS, TEST_REPEAT, [](size_t numberOfElements){
			Bitstream bitstream;
			while (numberOfElements--) {
				bitstream.add(numberOfElements % 2);
			}
		});
	}
	
	if (testByteBit) {
		std::cout << "Write byte bits: ";
		Test::performance(TEST_ITERATIONS, TEST_REPEAT, [](size_t numberOfElements){
			Bitstream bitstream;
			// bitstream.add(1);
			while (numberOfElements--) {
				bitstream.add(0xd2, 8);
			}
		});
	}
	
	// create random bitstream for reading
	Bitstream testStream;
	size_t fillRandom = TEST_ITERATIONS;
	while (fillRandom--)
		testStream.add( arc4random() % 2 );
	
	
	if (testRead) {
		std::cout << "Read single bit: ";
		Test::performance(TEST_ITERATIONS, TEST_REPEAT, [&testStream](size_t numberOfElements){
			size_t maxRead = testStream.numberOfBits() - 2;
			size_t idx = 0;
			while (numberOfElements--) {
				testStream.read(idx++);
				if (idx > maxRead)
					idx = 0;
			}
		});
	}
	
	if (testWriteFile) {
		std::cout << "Write file: ";
		Test::performance([&testStream] {
			testStream.saveToFile("data/writeOleg.txt");
		});
	}
}

// ################################################################
// #
// #  Main
// #
// ################################################################

int main(int argc, const char *argv[]) {
//	testImage();
	testChris();
//	testMarcel();
//	testMarv();
//	testOleg(true);
	
	return 0;
}
