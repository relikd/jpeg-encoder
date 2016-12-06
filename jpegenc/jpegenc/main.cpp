#include <iostream>
#include <stdlib.h>
#include "io/PPMLoader.hpp"
#include "converter/RGBToYCbCrConverter.hpp"
#include "converter/YCbCrToRGBConverter.hpp"
#include "helper/Test.hpp"
#include "Huffman.hpp"

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
	bitStream.saveToFile("out.txt");
	
	
	std::cout << "Testing, Bitstream" << std::endl;
	
	std::cout << "Write single bit: ";
	Test::performance(TEST_ITERATIONS, TEST_REPEAT, [](size_t numberOfElements){
		Bitstream bitstream;
		while (numberOfElements--) {
			bitstream.add(numberOfElements % 2);
		}
	});
	
	
	std::cout << "Write byte bits: ";
	Test::performance(TEST_ITERATIONS, TEST_REPEAT, [](size_t numberOfElements){
		Bitstream bitstream;
		// bitstream.add(1);
		while (numberOfElements--) {
			bitstream.add(0xd2, 8);
		}
	});
	
	
	// create random bitstream for reading
	Bitstream testStream;
	size_t fillRandom = TEST_ITERATIONS;
	while (fillRandom--)
		testStream.add( arc4random() % 2 );
	
	
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
	
	
	std::cout << "Write file: ";
	Test::performance([&testStream] {
		testStream.saveToFile("data/writeOleg.txt");
	});
	
	
	
	PPMLoader loader;
	auto image = loader.load("data/very_small.ppm");
	
	JPEGWriter writer;
	writer.writeJPEGImage(image, "Test.test.jpg");
}

std::vector<Symbol> getWord() {
	std::vector<Symbol> input;
	input.push_back(2);
	input.push_back(4);
	input.push_back(7);
	
	return input;
}

void addTestSymbol(int amount, int symbol, std::vector<int> &input) {
	for (int i = 0; i < amount; ++i) {
		input.push_back(symbol);
	}
}

std::vector<int> generateTestHuffman() {
	std::vector<int>input;
	addTestSymbol(6, 1, input);
	addTestSymbol(6, 2, input);
	addTestSymbol(8, 3, input);
	addTestSymbol(8, 4, input);
	addTestSymbol(12, 5, input);
	addTestSymbol(13, 6, input);
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
//	Node* rootTree = huffman.generateTree();
//	Node* rootTree = huffman.canonicalTree();
	Node* rootTree = huffman.lengthLimitedTree(16);
	
	rootTree->print();
	
	//std::map<Symbol, SymbolBits>* encodingTable = huffman.generateEncodingTable(rootTree);
	
//	Bitstream bitsteam;
//	std::vector<Symbol> word = getWord();
//	for (int i = 0; i < word.size(); ++i) {
//		SymbolBits bitsOfSymbol = encodingTable->at(word[i]);
//		bitsteam.add(bitsOfSymbol.bits, bitsOfSymbol.numberOfBits);
//	}
//	bitsteam.print();
	
}

// ################################################################
// #
// #  Main
// #
// ################################################################

int main(int argc, const char *argv[]) {
	
	testhuffmann();
	//testJPEGWriter();
	
	return 0;
}
