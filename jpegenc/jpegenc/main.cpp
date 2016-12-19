#include <iostream>
#include <stdlib.h>
#include "io/PPMLoader.hpp"
#include "converter/RGBToYCbCrConverter.hpp"
#include "converter/YCbCrToRGBConverter.hpp"
#include "helper/Test.hpp"
#include "Huffman.hpp"
#include "DCT.hpp"
#include "Arai.hpp"
#include <math.h>

#include "bitstream/Bitstream.hpp"

#include "segments/JPEGSegments.hpp"

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
	Test::performance([]{
		PPMLoader loader;
		auto image = loader.load("../data/singapore4k.test.ppm");
		
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
//	Test::performance(TEST_ITERATIONS, TEST_REPEAT, [](size_t numberOfElements){
//		Bitstream bitstream;
//		while (numberOfElements--) {
//			bitstream.add(numberOfElements % 2);
//		}
//	});
//	
//	
//	std::cout << "Write byte bits: ";
//	Test::performance(TEST_ITERATIONS, TEST_REPEAT, [](size_t numberOfElements){
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
//	Test::performance(TEST_ITERATIONS, TEST_REPEAT, [&testStream](size_t numberOfElements){
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
//	Test::performance([&testStream] {
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

void testDirectDCT() {
	Mat input;
	input.initiate((float[]){
		2, 2,
		2, 2
	}, 2, 2);
	
	Mat out = DCT::transform(input);
	
	out.print();
}

void testIDCT() {
	Mat input;
	input.initiate((float[]){
		2, 2, 2,
		2, 2, 2,
		2, 2, 2
	}, 3, 3);
	
	Mat out = DCT::transform2(input);
	std::cout << "DCT Mat:" << std::endl;
	out.print();
	
	Mat inverse = DCT::inverse(out);
	std::cout << "Inverse Mat:" << std::endl;
	inverse.print();
}

void testMat() {
	Mat a;
	a.initiate((float[]){
		1, 0, 0,
		0, 1, 0,
		0, 0, 1}, 3 , 3);
	
	Mat b;
	b.initiate((float[]){
		1, 2, 3,
		0, 1, 4,
		0, 5, 1}, 3 , 3);
	
	Mat c = a * b;
	c.print();
}

void testAraiLine()
{
    float *values = new float[8];
    
    values[0] = 1;
    values[1] = 7;
    values[2] = 3;
    values[3] = 4;
    values[4] = 5;
    values[5] = 4;
    values[6] = 3;
    values[7] = 2;

    Arai::transformLine(values);

    bool test = true;
    float tolerance = 0.0001;
    
    test = test && (fabsf(values[0] - (10.253f))     < tolerance);
    test = test && (fabsf(values[1] - (0.797218f))   < tolerance);
    test = test && (fabsf(values[2] - (-2.19761f))   < tolerance);
    test = test && (fabsf(values[3] - (-0.0377379f)) < tolerance);
    test = test && (fabsf(values[4] - (-1.76777f))   < tolerance);
    test = test && (fabsf(values[5] - (-2.75264f))   < tolerance);
    test = test && (fabsf(values[6] - (-2.53387f))   < tolerance);
    test = test && (fabsf(values[7] - (-1.13403f))   < tolerance);
    
    if ( test )
    {
        std::cout << "All values are correct." << std::endl;
    }
    else
    {
        std::cout << "Something went wrong." << std::endl;
    }
}

void testAraiMatrix()
{
    Mat matrix;
    
    matrix.initiate((float[]) {
        2, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0
    }, 8, 8);
    //matrix.print();

    std::cout << std::endl;
    
    matrix = Arai::transform(matrix);
    //matrix.print();
	
    std::cout << std::endl;

    matrix = DCT::inverse(matrix);
    matrix.print();
}

// ################################################################
// #
// #  Main
// #
// ################################################################

int main(int argc, const char *argv[]) {
	
	//testhuffmann();
    //testJPEGWriter();
	//testDirectDCT();
    //testIDCT();
	//testMat();
	//testImage();
    testAraiLine();
    //testAraiMatrix();
	
	return 0;
}
