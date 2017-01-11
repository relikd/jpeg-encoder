#include "GPUComposer.h"
#include "../helper/MemoryShifter.hpp"

bool GPUComposer::add(float* matrix, size_t width, size_t height)
{
//	if (width & 0b111 || height & 0b111) {
//		fputs("Error: Image size must be a factor of 8.\n", stderr);
//		return false;
//	}
	
	if (shouldClearStoredData) {
		// prepare everything
		cachedSize = 0;
		cacheInfo.clear();
		shouldClearStoredData = false;
	}
	
	// copy data to internal cache
	if (shouldRestructureData) {
		MemoryShifter::squeezeImageToBlockWith(&cache[cachedSize], matrix, width, height);
	} else {
		// copy data without modification
#if USE_MEMCPY
		memcpy(&cache[cachedSize], matrix, width * height * sizeof(float));
#else
		float *ptrC = &cache[cachedSize];
		size_t repeatPx = width * height;
		while (repeatPx--) {
			*(ptrC++) = *(matrix++);
		}
#endif
	}
	
	size_t px = width * height;
	cacheInfo.push_back(DATA_INFO(cachedSize, width, height)); // add before increase
	cachedSize += px;
	
	// if cache full, copy to GPU
	if (cachedSize > cachedSizeThreshold) {
		return true; // you have to flush manually
	}
	return false;
}

void reconstructData(float* &source, std::vector<DATA_INFO> &meta) {
	float *tmp = new float[cacheSizeMax];
	size_t imageCount =  meta.size();
	while (imageCount--) {
		DATA_INFO info = meta[imageCount];
		MemoryShifter::restoreSqueezedImage(tmp + info.offset, source + info.offset, info.width, info.height);
	}
	delete [] source;
	source = tmp;
}

void GPUComposer::flush() {
	if (cachedSize > 0) {
		if (shouldRestructureData) {
			func(cache, 8, cachedSize / 8); // 8-float width alignment
			reconstructData(cache, cacheInfo);
		} else {
			// since all sizes are equal, just take the first one
			func(cache, cacheInfo.front().width, cacheInfo.front().height);
		}
		shouldClearStoredData = true;
	}
}
