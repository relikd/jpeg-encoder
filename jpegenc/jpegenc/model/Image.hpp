#ifndef Image_hpp
#define Image_hpp

#include "Channel.hpp"
#include <iostream>

typedef enum {
	ColorSpaceRGB,
	ColorSpaceYCbCr,
} ColorSpace;

struct Image {
	Dimension imageSize;
	ColorSpace colorSpace;

	Channel *channel1;
	Channel *channel2;
	Channel *channel3;

	Image(Dimension dim) : imageSize(dim) {
		channel1 = new Channel(imageSize);
		channel2 = new Channel(imageSize);
		channel3 = new Channel(imageSize);
	}

	~Image() {
		delete channel1;
		delete channel2;
		delete channel3;
	}

	void print(bool useIntValues = true);
	void reduceBySubSample(size_t xdiv, size_t ydiv);
	void reduceByAverage(size_t xdiv, size_t ydiv);
	void seekAllChannelToStart(ChannelSeekType type);
	void setReadingRuleForAllChannel();
};

#endif /* Image_hpp */
