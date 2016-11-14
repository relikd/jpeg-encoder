//
//  BitstreamChris.hpp
//  jpegenc
//
//  Created by Christian Braun on 08/11/16.
//  Copyright © 2016 FHWS. All rights reserved.
//

#ifndef BitstreamChris_hpp
#define BitstreamChris_hpp

#include <stdio.h>
#include <iostream>
#include <vector>

const int MAX_BIT_INDEX = 31;

struct BitStream {
	
	int bitIndex = 31;
	size_t blockIndex;
	size_t bufferIndex = 0;
	size_t bufferSize;
	std::vector<uint32_t*> blocks;
	
	BitStream(size_t bufferSizeInBytes = 10000) : bufferSize(bufferSizeInBytes / 4){
		addBlock();
		++blockIndex;
	}
	
	void add(bool bit);
	bool read(size_t index);
	void print();
	
private:
	void addBlock();
};

#endif /* BitstreamChris_hpp */
