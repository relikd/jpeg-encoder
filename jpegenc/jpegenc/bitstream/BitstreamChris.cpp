//
//  BitstreamChris.cpp
//  jpegenc
//
//  Created by Christian Braun on 08/11/16.
//  Copyright © 2016 FHWS. All rights reserved.
//

#include <bitset>
#include "BitstreamChris.hpp"

unsigned int Endian_DWord_Conversion(unsigned long int dword)
{
	return ((dword>>24)&0x000000FF) | ((dword>>8)&0x0000FF00) | ((dword<<8)&0x00FF0000) | ((dword<<24)&0xFF000000);

}

void BitStream::add(bool bit){
	
	if(bitIndex == -1) {
		bitIndex = MAX_BIT_INDEX;
		++bufferIndex;
		
		if(bufferIndex == bufferSize) {
			bufferIndex = 0;
			addBlock();
		}
	}
	

	if(bit)
		blocks[blockIndex][bufferIndex] ^= bit << bitIndex ;
	
	--bitIndex;
}

void BitStream::add(char* bits, size_t length) {
	auto castesBits = (uint32_t*) bits;
	for(size_t i = 0; i < length / 4; ++i) {
		blocks[blockIndex][bufferIndex] = Endian_DWord_Conversion( castesBits[i]);
		++bufferIndex;
		
		if(bufferIndex == bufferSize) {
			bufferIndex = 0;
			addBlock();
		}
	}
	
	bitIndex = -1;
}


bool BitStream::read(size_t index){
	auto given_intIndex = index / (MAX_BIT_INDEX + 1);
	auto given_blockIndex = given_intIndex / bufferSize;
	auto given_bufferIndex = given_intIndex - bufferSize * given_blockIndex;
	auto given_bitIndex = MAX_BIT_INDEX - index % (MAX_BIT_INDEX + 1);
	
	std::cout << given_blockIndex << " " << given_bufferIndex << " " << given_bitIndex << std::endl;
	return (blocks[given_blockIndex][given_bufferIndex] >> given_bitIndex) & 1;
}

void BitStream::print(){
	std::cout << blockIndex << " " << bufferIndex << " " << bitIndex << std::endl;
	for (size_t i = 0; i <= blockIndex; ++i) {
		for (int k = 0;  k <= bufferSize; ++k) {
			std::cout << i << " " << k << " " << std::endl;
			std::cout << std::bitset<MAX_BIT_INDEX + 1>(blocks[i][k]) << std::endl;
			
			if (i == blockIndex && k >= bufferIndex && bitIndex <= MAX_BIT_INDEX)
				break;
		}
	}
}

void BitStream::addBlock() {
	blocks.push_back(new uint32_t[bufferSize]);
	++blockIndex;
}



