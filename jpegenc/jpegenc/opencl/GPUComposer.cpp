#include "GPUComposer.h"

#define RESTRUCTURE_DATA 1 // can be 0 if all images have the same size

#if RESTRUCTURE_DATA
/**
 * Copy data with 8-float width alignment. Make one image with width = 8 and height = very very long
 */
void prepareData(float* cachePtr, float* dataPtr, size_t w, size_t h) {
	const unsigned char copySize = 8 * sizeof(float);
	size_t x, y, eight;
	y = h / 8;
	while (y--) {
		x = w / 8;
		while (x--) {
			eight = 8;
			while (eight--) {
				memcpy(cachePtr, dataPtr, copySize);
				cachePtr += 8;
				dataPtr += w;
			}
			dataPtr -= 8 * w;
			dataPtr += 8;
		}
		dataPtr += 7 * w;
	}
}

void reconstructData(float* &data, std::vector<DATA_INFO> &meta) {
	float *tmp = new float[cacheSizeMax];
	size_t imageCount =  meta.size();
	while (imageCount--) {
		DATA_INFO info = meta[imageCount];
		float *source = &data[ info.offset ];
		float *dest = &tmp[ info.offset ];
		
		size_t width = info.width;
		
		// copy data
		size_t x, y, eight;
		const unsigned char copySize = 8 * sizeof(float);
		
		y = info.height / 8;
		while (y--) {
			x = width / 8;
			while (x--) {
				eight = 8;
				while (eight--) {
					memcpy(dest, source, copySize);
					source += 8;
					dest += width;
				}
				dest -= 8 * width;
				dest += 8;
			}
			dest += 7 * width;
		}
	}
	delete [] data;
	data = tmp;
}
#endif

bool GPUComposer::add(float* &matrix, size_t width, size_t height)
{
	if (width & 0b111 || height & 0b111) {
		fputs("Error: Image size must be a factor of 8.\n", stderr);
		return false;
	}
	
	if (shouldClearStoredData) {
		// prepare everything
		cachedSize = 0;
		cacheInfo.clear();
		shouldClearStoredData = false;
	}
	
	// add data to cache first, then update internal counters
#if RESTRUCTURE_DATA
	prepareData(&cache[cachedSize], matrix, width, height);
#else
	memcpy(&cache[cachedSize], matrix, sizeof(float) * width * height);
#endif
	
	size_t px = width * height;
	cacheInfo.push_back(DATA_INFO(cachedSize, width, height)); // add before increase
	cachedSize += px;
	
	// if cache full, copy to GPU
	if (cachedSize > cachedSizeThreshold) {
		return true; // you have to flush manually
	}
	return false;
}

void GPUComposer::flush() {
	if (cachedSize > 0) {
		func(cache, 8, cachedSize / 8); // at this point always a multiple of 8
#if RESTRUCTURE_DATA
		reconstructData(cache, cacheInfo);
#endif
		shouldClearStoredData = true;
	}
}
