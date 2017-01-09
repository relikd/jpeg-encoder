#include "GPUComposer.h"

#define USE_MEMCPY 1

/**
 * Copy data with 8-float width alignment. Make one image with width = 8 and height = very very long
 */
void prepareData(float* cachePtr, float* dataPtr, size_t w, size_t h) {
	size_t x, y;
#if USE_MEMCPY
	const size_t copySize = 8 * sizeof(float);
#else
	const size_t smallSkip = (w - 8);
#endif
	const size_t largeSkip = (8 * w) - 8;
	
	y = h / 8;
	while (y--) {
		x = w / 8;
		while (x--) {
			unsigned short rows = 8;
			while (rows--) {
#if USE_MEMCPY
				memcpy(cachePtr, dataPtr, copySize);
				cachePtr += 8;
				dataPtr += w;
#else
				unsigned short cols = 8;
				while (cols--) {
					*(cachePtr++) = *(dataPtr++);
				}
				dataPtr += smallSkip;
#endif
			}
			dataPtr -= largeSkip;
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
		size_t x, y;
#if USE_MEMCPY
		const size_t copySize = 8 * sizeof(float);
#else
		const size_t smallSkip = (width - 8);
#endif
		const size_t largeSkip = (8 * width) - 8;
		
		y = info.height / 8;
		while (y--) {
			x = width / 8;
			while (x--) {
				unsigned short rows = 8;
				while (rows--) {
#if USE_MEMCPY
					memcpy(dest, source, copySize);
					source += 8;
					dest += width;
#else
					unsigned short cols = 8;
					while (cols--) {
						*(dest++) = *(source++); // notice subtle difference here, compared to prepareData();
					}
					dest += smallSkip;
#endif
				}
				dest -= largeSkip;
			}
			dest += 7 * width;
		}
	}
	delete [] data;
	data = tmp;
}

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
	if (shouldRestructureData) {
		prepareData(&cache[cachedSize], matrix, width, height);
	} else {
		// copy data without modification
#if USE_MEMCPY
		memcpy(&cache[cachedSize], matrix, width * height * sizeof(float));
#else
		float *ptrC = &cache[cachedSize];
		float *ptrM = &matrix[0];
		size_t repeatPx = width * height;
		while (repeatPx--) {
			*(ptrC++) = *(ptrM++);
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
