#include "GPUComposer.h"

/**
 * Copy data with 8-float width alignment. Make one image with width = 8 and height = very very long
 */
void prepareData(float* cachePtr, float* dataPtr, size_t w, size_t h) {
	size_t x, y, rows, cols;
	const size_t smallSkip = (w - 8);
	const size_t largeSkip = (8 * w) - 8;
	
	y = h / 8;
	while (y--) {
		x = w / 8;
		while (x--) {
			rows = 8;
			while (rows--) {
				cols = 8;
				while (cols--) {
					*(cachePtr++) = *(dataPtr++);
				}
				dataPtr += smallSkip;
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
		size_t x, y, rows, cols;
		const size_t smallSkip = (width - 8);
		const size_t largeSkip = (8 * width) - 8;
		
		y = info.height / 8;
		while (y--) {
			x = width / 8;
			while (x--) {
				rows = 8;
				while (rows--) {
					cols = 8;
					while (cols--) {
						*(dest++) = *(source++); // notice subtle difference here, compared to prepareData();
					}
					dest += smallSkip;
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
		float *ptrC = &cache[cachedSize];
		float *ptrM = &matrix[0];
		size_t repeatPx = width * height;
		while (repeatPx--) {
			*(ptrC++) = *(ptrM++);
		}
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
