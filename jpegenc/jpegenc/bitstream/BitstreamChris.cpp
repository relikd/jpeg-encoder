//
//  BitstreamChris.cpp
//  jpegenc
//
//  Created by Christian Braun on 08/11/16.
//  Copyright © 2016 FHWS. All rights reserved.
//

#include <bitset>
#include "BitstreamChris.hpp"

void BitStream::add(bool bit){
	
	if(bitIndex == -1) {
		bitIndex = 31;
		++bufferIndex;
	}
	
	if(bufferIndex == bufferSize) {
		bufferIndex = 0;
		blocks.push_back(new uint32_t[bufferSize]);
		++blockIndex;
	}
	
	if(bit)
		blocks[blockIndex][bufferIndex] ^= bit << bitIndex ;
	
	--bitIndex;
}

bool BitStream::read(size_t index){
	auto given_intIndex = index / 32;
	auto given_blockIndex = given_intIndex / bufferSize;
	auto given_bufferIndex = given_intIndex - bufferSize * given_blockIndex;
	auto given_bitIndex = 31 - index % 32;
	
	std::cout << given_blockIndex << " " << given_bufferIndex << " " << given_bitIndex << std::endl;
	return (blocks[given_blockIndex][given_bufferIndex] >> given_bitIndex) & 1;
}

void BitStream::print(){
	std::cout << blockIndex << " " << bufferIndex << " " << bitIndex << std::endl;
	for (size_t i = 0; i <= blockIndex; ++i) {
		for (int k = 0;  k <= bufferSize; ++k) {
			std::cout << i << " " << k << " " << std::endl;
			std::cout << std::bitset<32>(blocks[i][k]) << std::endl;
			
			if (i == blockIndex && k >= bufferIndex && bitIndex <= 31)
				break;
		}
	}
}
