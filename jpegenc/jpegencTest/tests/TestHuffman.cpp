//
//  TestHuffman.cpp
//  jpegenc
//
//  Created by Christian Braun on 03/01/17.
//  Copyright © 2017 FHWS. All rights reserved.
//

#include <stdio.h>
#include <vector>
#include "catch.hpp"
#include "Huffman.hpp"

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


TEST_CASE("Test Huffman Tree and encoding table generation") {
	// This will be caled before each section of the testcase
	std::vector<int> testData = generateTestHuffman();
	Huffman huffman(testData);
	
	SECTION("Generate standard tree") {
		auto root = huffman.standardTree();
		REQUIRE(!root->isLeaf());
	}
	
	SECTION("Generate canonical encoding table") {
		auto table = huffman.canonicalEncoding();
		REQUIRE(table.size() == 6);
		REQUIRE(table[6].numberOfBits == 2);
	}
	
	SECTION("Generate canonical length limited encoding table") {
		auto table = huffman.canonicalEncoding(3);
		REQUIRE(table.size() == 6);
		REQUIRE(table[6].numberOfBits == 2);
	}
}

