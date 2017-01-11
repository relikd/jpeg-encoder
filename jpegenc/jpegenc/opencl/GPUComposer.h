#ifndef GPUComposer_hpp
#define GPUComposer_hpp

#include <vector>
#include <functional>

typedef std::function<void(float*&,size_t,size_t)> gpu_function;

static const size_t cachedSizeThreshold = 500 * 1024 * 1024 / sizeof(float);
static const size_t cacheSizeMax = cachedSizeThreshold + 67108864; // alloc enough space for an additional 8192 x 8192 px image

/**
 * Info about composed image data container
 */
struct DATA_INFO {
	size_t offset;
	size_t width, height;
	
	DATA_INFO(size_t o, size_t w, size_t h) : offset(o), width(w), height(h) {};
};

/**
 * Wait for a couple images to reduce the CPU-GPU communication
 */
class GPUComposer {
	size_t cachedSize = 0;
	bool shouldClearStoredData = true;
	const bool shouldRestructureData = false; // massive performance increase if true!
	
public:
	float *cache = new float[cacheSizeMax];
	std::vector<DATA_INFO> cacheInfo;
	gpu_function func;
	
	GPUComposer(gpu_function f, bool allImagesHaveSameSize = false) : func(f), shouldRestructureData(!allImagesHaveSameSize) {};
	~GPUComposer() {
		delete[] cache;
	};
	
	bool add(float* matrix, size_t width, size_t height);
	void flush();
};

#endif /* GPUComposer_hpp */
