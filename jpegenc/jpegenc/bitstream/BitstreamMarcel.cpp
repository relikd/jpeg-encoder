#include <iostream>
#include <cmath>
#include "BitstreamMarcel.hpp"

void BitstreamMarcel::add( bool bit ) {

	allocNewByteIfNeeded();
	allocNewBlockIfNeeded();
	// cout << "block " << block_index << " byte " << byte_index << " bit " << bit_index << endl;

	if (bit) {
		blocks[block_index][byte_index] ^= 1 << (7-bit_index);
	}
	++bit_index;

}
void BitstreamMarcel::add( char byte, size_t amount ) {

}
void BitstreamMarcel::print(){
	cout << "BitStream: " << endl;
	for (int i = 0; i <= block_index; ++i) {
		char* currentBlock = blocks[i];

		for (int j = 0; j <= block_size; ++j) {
			cout << (int)currentBlock[j] << " ";
		}

		cout << endl;
	}

}

bool BitstreamMarcel::read( size_t index ){
	size_t blockNumber = ceil((float) index / (float) block_size);
	size_t blockIndex = index % (block_size * (blockNumber -1));

	cout << "blockNumber " << blockNumber << " blockIndex " << blockIndex << endl;

	return blocks[blockNumber-1][blockIndex-1];
}

void BitstreamMarcel::allocNewByteIfNeeded() {
	if (bit_index == 8) {
		++byte_index;
		bit_index = 0;
	}
}

void BitstreamMarcel::allocNewBlockIfNeeded() {
	if (byte_index == block_size) {
		blocks.push_back(new char[block_size]);
		++block_index;
		byte_index = 0;
	}
}
