#ifndef JPEG_ENCODER_BITSTREAMMARCEL_H
#define JPEG_ENCODER_BITSTREAMMARCEL_H

#include <vector>
#include <iostream>

using namespace std;

class BitstreamMarcel {

	size_t block_index = 0;
	size_t byte_index = 0;
	size_t bit_index = 0;
	size_t block_size = 10;
	vector<char *> blocks;

public:
	BitstreamMarcel() {
		char* initialBlock = new char[block_size];
		blocks.push_back(initialBlock);
	}

	~BitstreamMarcel() {

	}

	void add( bool bit );
	void add( char byte, size_t amount );
	void print();
	bool read( size_t index);

private:
	void allocNewByteIfNeeded();
	void allocNewBlockIfNeeded();
};

#endif


