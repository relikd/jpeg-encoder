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
	
	size_t px = width * height;
	
	// copy data to internal cache
	if (shouldRestructureData) {
		MemoryShifter::squeezeImageToBlockWith(&cache[cachedSize], matrix, width, height);
	} else {
		// copy data without modification
		float *ptrCache = &cache[cachedSize];
#if USE_MEMCPY
		memcpy(ptrCache, matrix, px * sizeof(float));
#else
		size_t i = px;
		while (i--) {
			*(ptrCache++) = *(matrix++);
		}
#endif
	}
	
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
			size_t width = cacheInfo.front().width;
			func(cache, width, cachedSize / width);
		}
		shouldClearStoredData = true;
	}
}
