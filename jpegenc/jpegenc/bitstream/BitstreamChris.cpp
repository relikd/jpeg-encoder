//
//  BitstreamChris.cpp
//  jpegenc
//
//  Created by Christian Braun on 08/11/16.
//  Copyright © 2016 FHWS. All rights reserved.
//

#include "BitstreamChris.hpp"

void BitStream::add(bool bit){
	// 2147483616 represents 1111111111111111111111111100000 which works as a bitmask
	// to get the 5 bits at the end which contain the current position in the given int
	//	if(bit)
	
	
	blocks[blockIndex][bitIndex / 32] ^= bit << (31 - (bitIndex ^ 2147483616)) ;
	blockIndex = bitIndex / 32;
	
	//	std::cout << blockIndex << " " << bitIndex / 32 << std::endl;
	++bitIndex;
	
	if(bitIndex % bufferSize == 0) {
		++blockIndex;
	}
}

bool BitStream::read(size_t index){
	return false;
}

void BitStream::print(){
	for (size_t i = 0; i <= blockIndex; ++i) {
		for (int i = 0;  i < bufferSize; ++i) {
			std::cout << blocks[0][i] << std::endl;
		}
	}
}
