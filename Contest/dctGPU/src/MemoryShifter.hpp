#ifndef MemoryShifter_hpp
#define MemoryShifter_hpp

#include <memory>
#define USE_MEMCPY 1

class MemoryShifter {
public:
	template <class T>
	static void squeezeImageToBlockWith(T* destination, T* source, size_t w, size_t h) {
		size_t x, y;
#if USE_MEMCPY
		const size_t copySize = 8 * sizeof(T);
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
					memcpy(destination, source, copySize);
					destination += 8;
					source += w;
#else
					unsigned short cols = 8;
					while (cols--) {
						*(destination++) = *(source++);
					}
					source += smallSkip;
#endif
				}
				source -= largeSkip;
			}
			source += 7 * w;
		}
	}
	
	template <class T>
	static void restoreSqueezedImage(T* destination, T* source, size_t w, size_t h) {
		size_t x, y;
#if USE_MEMCPY
		const size_t copySize = 8 * sizeof(T);
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
					memcpy(destination, source, copySize);
					source += 8;
					destination += w;
#else
					unsigned short cols = 8;
					while (cols--) {
						*(destination++) = *(source++); // notice subtle difference here, compared to prepareData();
					}
					destination += smallSkip;
#endif
				}
				destination -= largeSkip;
			}
			destination += 7 * w;
		}
	}
};


#endif /* MemoryShifter_hpp */
