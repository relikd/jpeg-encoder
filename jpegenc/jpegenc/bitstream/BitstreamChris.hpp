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

struct BitStream {
	
	size_t blockIndex = 0;
	size_t bufferIndex = 0;
	size_t bitIndex = 0;
	size_t bufferSize;
	std::vector<uint32_t*> blocks;
	
	BitStream(size_t bufferSizeInBytes = 10000) : bufferSize(bufferSizeInBytes / 4){
		blocks.push_back(new uint32_t[bufferSize]);
	}
	
	void add(bool bit);
	bool read(size_t index);
	void print();
};

#endif /* BitstreamChris_hpp */
