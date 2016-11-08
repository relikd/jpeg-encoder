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
	return false;
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
