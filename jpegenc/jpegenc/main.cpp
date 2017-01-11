#include <iostream>
#include <stdlib.h>
#include "io/PPMLoader.hpp"
#include "converter/RGBToYCbCrConverter.hpp"
#include "converter/YCbCrToRGBConverter.hpp"
#include "helper/Performance.hpp"
#include "huffman/Huffman.hpp"
#include "bitstream/Bitstream.hpp"
#include "segments/JPEGSegments.hpp"
#include "speedcontest/SpeedContest.hpp"
#include "opencl/OCL_DCT.h" // only for GPU_SETTINGS

using namespace JPEGSegments;

#define TEST_ITERATIONS 10000000
#define TEST_REPEAT 10

std::vector<int> generateTestHuffman();

//  ---------------------------------------------------------------
// |
// |  PPM Image Processing
// |
//  ---------------------------------------------------------------

void testImage() {
	std::cout << "Loading image ..." << std::endl;
	Performance::time([]{
		PPMLoader loader;
		auto image = loader.load("../data/singapore4k.test.ppm");
		
		RGBToYCbCrConverter converter1;
		converter1.convert(image);
		
		//image->print();
		
//		image->channel2->reduceBySubSampling(32, 32);
//		image->channel2 = Channel::enlarge(image->channel2, 32, 32); // this is only needed to convert back to RGB
		
		YCbCrToRGBConverter converter2;
		converter2.convert(image);
		
		Performance::time([&loader, &image]{
			loader.write("../data/output.test.ppm", image);
		});
	});
}

//  ---------------------------------------------------------------
// |
// |  JPEG Writer
// |
//  ---------------------------------------------------------------

void testJPEGWriter() {
	
//	Bitstream bitStream;
//	bitStream.add(1);
//	bitStream.add(0);
//	bitStream.add(1);
//	bitStream.add(1);
//	bitStream.add(0);
//	bitStream.print();
//	bitStream.fillup(1);
//	bitStream.print();
//	bitStream.saveToFile("out.txt");
//	
//	
//	std::cout << "Testing, Bitstream" << std::endl;
//	
//	std::cout << "Write single bit: ";
//	Performance::repeat(TEST_ITERATIONS, TEST_REPEAT, [](size_t numberOfElements){
//		Bitstream bitstream;
//		while (numberOfElements--) {
//			bitstream.add(numberOfElements % 2);
//		}
//	});
//	
//	
//	std::cout << "Write byte bits: ";
//	Performance::repeat(TEST_ITERATIONS, TEST_REPEAT, [](size_t numberOfElements){
//		Bitstream bitstream;
//		// bitstream.add(1);
//		while (numberOfElements--) {
//			bitstream.add(0xd2, 8);
//		}
//	});
//	
//	
//	// create random bitstream for reading
//	Bitstream testStream;
//	size_t fillRandom = TEST_ITERATIONS;
//	while (fillRandom--)
//		testStream.add( arc4random() % 2 );
//	
//	
//	std::cout << "Read single bit: ";
//	Performance::repeat(TEST_ITERATIONS, TEST_REPEAT, [&testStream](size_t numberOfElements){
//		size_t maxRead = testStream.numberOfBits() - 2;
//		size_t idx = 0;
//		while (numberOfElements--) {
//			testStream.read(idx++);
//			if (idx > maxRead)
//				idx = 0;
//		}
//	});
//	
//	
//	std::cout << "Write file: ";
//	Performance::time([&testStream] {
//		testStream.saveToFile("../data/writeOleg.txt");
//	});
//	
	
	
	PPMLoader loader;
	auto image = loader.load("../data/very_small.ppm");
	
	JPEGWriter writer;
	auto testData = generateTestHuffman();
	Huffman huffman(testData);
	
	
	writer.writeJPEGImage(image, "../data/Test1.test.jpg", huffman.canonicalEncoding(16));
}

std::vector<Symbol> getWord() {
	std::vector<Symbol> input;
	input.push_back(1);
	input.push_back(4);
	input.push_back(6);
	
	return input;
}

void addTestSymbol(int amount, int symbol, std::vector<int> &input) {
	for (int i = 0; i < amount; ++i) {
		input.push_back(symbol);
	}
}

std::vector<int> generateTestHuffman() {
	std::vector<int>input;
	addTestSymbol(5, 1, input);
	addTestSymbol(12, 5, input);
	addTestSymbol(13, 6, input);
	addTestSymbol(6, 2, input);
	addTestSymbol(8, 3, input);
	addTestSymbol(8, 4, input);
	return input;
}

std::vector<int> generateTestHuffman2() {
	std::vector<int>input;
	addTestSymbol(5, 1, input);
	addTestSymbol(5, 2, input);
	addTestSymbol(6, 3, input);
	addTestSymbol(11, 4, input);
	addTestSymbol(12, 5, input);
	addTestSymbol(12, 6, input);
	addTestSymbol(26, 7, input);
	return input;
}

void testhuffman() {
	std::vector<Symbol> testData;
	
	auto input = generateTestHuffman();
	
	
	Huffman huffman = Huffman(input);
	huffman.preventAllOnesPath(true);
	//	auto encodingTable = huffman.canonicalEncoding();
	auto encodingTable = huffman.canonicalEncoding(4);
	//	Node* rootTree = huffman.standardTree();
	Node* rootTree = huffman.treeFromEncodingTable(encodingTable);
	
	
	for (auto pair: encodingTable) {
		std::cout << pair.first << ": " << pair.second << std::endl;
	}
	rootTree->print();
	rootTree->exportTree();
	Bitstream bitsteam;
	std::vector<Symbol> word = getWord();
	for (int i = 0; i < word.size(); ++i) {
		Encoding enc = encodingTable.at(word[i]);
		std::cout << "füge " << enc << " hinzu (" << word[i] << ")" << std::endl;
		bitsteam.add(enc.code, enc.numberOfBits);
	}
	bitsteam.print();
}


// ################################################################
// #
// #  Main
// #
// ################################################################

int main(int argc, const char *argv[]) {
	
	//testhuffman();
    //testJPEGWriter();
//	testImage();
	
	double testTime = 10.0F;
	bool testSkipCPU = false;
	bool testSkipGPU = false;
	
	
	int i = argc;
	while (--i) { // skip the first param, which is the path of this executable
		const char* param = argv[i];
		if (param[0] == '-') {
			if (strncmp(param, "-valid", 6) == 0) // -valid111 (three 1 for three bool parameter)
			{
				long validateParam = strtol(param + 6, NULL, 2);
				SpeedContest::testForCorrectness(validateParam & 1, validateParam & 2, validateParam & 4);
				exit(EXIT_SUCCESS);
			}
			else if (strncmp(param, "-gpu", 4) == 0) // -gpu0, -gpu1, -gpuN0, -gpuN1
			{
				long gpu = -1;
				if (param[4] == 'N' || param[4] == 'n') {
					OCL_DCT::forceNvidiaPlatform(true);
					gpu = strtol(param + 5, NULL, 10);
				} else {
					gpu = strtol(param + 4, NULL, 10);
				}
				OCL_DCT::setPreferedGPU((int)gpu);
			}
			else if (strcmp(param, "-nocpu") == 0)
			{
				testSkipCPU = true;
			}
			else if (strcmp(param, "-nogpu") == 0)
			{
				testSkipGPU = true;
			}
		} else {
			double tmp = strtod(param, NULL); // see if time provided, otherwise default to 10
			if (tmp > 0.5) testTime = tmp;
		}
	}
	
	printf("Starting Performance Test with %1.1fs\n", testTime);
	SpeedContest::run(testTime, testSkipCPU, testSkipGPU);
	
	return 0;
}
