#ifndef Image_hpp
#define Image_hpp

#include <stdlib.h>
#include <stdio.h>

struct Channel {
	size_t channelSize;
	size_t *values;

	Channel(size_t size = 0) : channelSize(size) {
		values = new size_t[size];
	}

	~Channel() {
		delete[] values;
	}

	size_t getValue(size_t x, size_t y, size_t width, size_t height);
	size_t getValue(size_t index, size_t numberOfPixels);
	void setValue(size_t x, size_t y, size_t width, size_t height, size_t value);
	void setValue(size_t index, size_t value);
	void reduceBySubSampling(size_t stepWidth);
	void reduceByAveraging(size_t stepWidth);
};

typedef enum {
	ColorSpaceRGB,
	ColorSpaceYCbCr,
} ColorSpace;

struct Image {
	size_t width, height;
	size_t numberOfPixels;
	ColorSpace colorSpace;

	Channel *channel1;
	Channel *channel2;
	Channel *channel3;

	Image(size_t width, size_t height) : width(width), height(height), numberOfPixels(width * height) {
		channel1 = new Channel(numberOfPixels);
		channel2 = new Channel(numberOfPixels);
		channel3 = new Channel(numberOfPixels);
	}

	~Image() {
		delete channel1;
		delete channel2;
		delete channel3;
	}

	size_t getIndex(size_t x, size_t y) const;

	size_t getValueFromChannel1(size_t x, size_t y);

	size_t getValueFromChannel2(size_t x, size_t y);

	size_t getValueFromChannel3(size_t x, size_t y);

	size_t getValueFromChannel1(size_t index);

	size_t getValueFromChannel2(size_t index);

	size_t getValueFromChannel3(size_t index);

	void setValueOnChannel1(size_t x, size_t y, size_t value);

	void setValueOnChannel2(size_t x, size_t y, size_t value);

	void setValueOnChannel3(size_t x, size_t y, size_t value);

	void setValueOnChannel1(size_t index, size_t value);

	void setValueOnChannel2(size_t index, size_t value);

	void setValueOnChannel3(size_t index, size_t value);

	void print();

	void reduceBySubSamplingChannel1(size_t stepWidth);

	void reduceBySubSamplingChannel2(size_t stepWidth);

	void reduceBySubSamplingChannel3(size_t stepWidth);

	void reduceByAveragingChannel1(size_t stepWidth);

	void reduceByAveragingChannel2(size_t stepWidth);

	void reduceByAveragingChannel3(size_t stepWidth);
};

#endif /* Image_hpp */
