#include <iostream>
#include <stdlib.h>
#include "io/PPMLoader.hpp"
#include "converter/RGBToYCbCrConverter.hpp"
#include "converter/YCbCrToRGBConverter.hpp"
#include "helper/Performance.hpp"
#include "huffmann/Huffman.hpp"
#include "bitstream/Bitstream.hpp"
#include "segments/JPEGSegments.hpp"
#include "speedcontest/SpeedContest.hpp"

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

void testhuffmann() {
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

float* generateBlockMatrix(size_t w, size_t h) {
	float data[8] = {1, 7, 3, 4, 5, 4, 3, 2}; // generate our well known test matrix
	float *vls = new float[w * h];
	for (int y = 0; y < h; y++) {
		for (int x = 0; x < w; x++) {
			float &u = vls[y*w + x];
			if (y % 8 == 0) {
				u = data[x%8];
			} else {
				if (x % 8 == 0) {
					u = data[y%8];
				} else {
					u = 0;
				}
			}
		}
	}
	return vls;
}

void printFloatMatrix(float* &mat, size_t w, size_t h) {
	for (int i = 0; i < w * h; ++i) {
		if (i % (w*8) == 0)
			printf("\n");
		printf("%1.3f  ", mat[i]);
		if (i % w == 7)
			printf("   ");
		if (i % w == w-1)
			printf("\n");
	}
}

void testFloatMatrixArrayDCT() {
	size_t width = 256, height = 256;
	float *vls = generateBlockMatrix(width, height);
	vls[8] = 0;
	vls[8+1] = 4;
	vls[8+width] = 4;
	vls[2*width + 1] = 8;
	vls[12*width + 1] = 1; // modify some valuesto get different results
	float *out = new float[width * height];
	
//	DCT::transform(vls, out, width, height);
//	printFloatMatrix(out, width, height);
	
//	Arai::transformInlineTranspose(vls, width, height);
//	printFloatMatrix(vls, width, height);
	
	SpeedContest::run(1, vls, width, height); // 1 second
}

// ################################################################
// #
// #  Main
// #
// ################################################################

int main(int argc, const char *argv[]) {
	
	//testhuffmann();
    //testJPEGWriter();
//	testImage();
	testFloatMatrixArrayDCT();
	
	return 0;
}
