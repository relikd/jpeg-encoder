#ifndef Channel_hpp
#define Channel_hpp

#include <functional>

#define START_INDEX 0

typedef float color;
typedef enum {
	ChannelSeekRead,
	ChannelSeekWrite,
	ChannelSeekBoth
} ChannelSeekType;


struct Dimension {
	size_t width, height;
	size_t pixelCount;
	
	Dimension(size_t w, size_t h) : width(w), height(h), pixelCount(w * h) {}
};


class Channel {
private:
	// Initial write
	unsigned short currentBlockSize = 16;
	size_t originalWidth = 0, originalHeight = 0;
	unsigned short blockAddCols = 0, blockAddRows = 0;
	
	// write
	color *writePointer;
	size_t currentWriteRow, currentWriteColumn;
	
	// read
	color *readPointer;
	size_t subsampleReadSkipX = 1, subsampleReadSkipY = 1;
	size_t countSkipX = 0, countSkipY = 0;
	size_t remainingReadInLine = 0;
	bool shouldReadSubsampleX = false, shouldReadSubsampleY = false;
	
public:
	Dimension imageSize;
	color *values;
	
	// Constructor
	Channel(Dimension dim, unsigned short blockSize = 8);
	static Channel* enlarge(Channel *base, size_t xMultiply, size_t yMultiply);
	
	~Channel() {
		delete[] values;
	}
	
	inline size_t numberOfPixel() { return imageSize.pixelCount; }
	
	// Read & Write
	color& readNextValue();
	void appendValue(const color &value);
	void appendValueAndFillupBlocks(const color &value);
	
	// Conditional Reading / Rules
	void seekTo(ChannelSeekType type, size_t index = START_INDEX);
	void setReadIndexAccessMappingRule(Dimension mapping_size);
	void deleteReadIndexAccessMappingRule();
	
	// Subsampling & Averaging
	void reduceBySubSampling(size_t stepWidth, size_t stepHeight=1);
	void reduceByAveraging(size_t stepWidth, size_t stepHeight=1);
	
private:
	inline void setImageSize(Dimension dim, unsigned short blockSize = 1);
	inline void fillupValuesByDuplicating();
	
	color& readNextSubsampleValue();
	
	void reduceWithFunction(size_t stepWidth, size_t stepHeight, std::function<color(size_t, size_t)>);
};

#endif /* Channel_hpp */
