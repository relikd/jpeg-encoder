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
	void reduceBySubSample(unsigned short xdiv, unsigned short ydiv);
	void reduceByAverage(unsigned short xdiv, unsigned short ydiv);
	void seekAllChannelToStart(ChannelSeekType type);
	void setReadingRuleForAllChannel();
};

struct ChannelData {
    Channel *channel1;
    Channel *channel2;
    Channel *channel3;
    
    ChannelData(std::shared_ptr<Image> image) {
        // Copy channel data from image
        channel1 = Channel::enlarge(image->channel1, 1, 1);
        channel2 = Channel::enlarge(image->channel2, 1, 1);
        channel3 = Channel::enlarge(image->channel3, 1, 1);
    }

    ~ChannelData() {
        delete channel1;
        delete channel2;
        delete channel3;
    }
};

#endif /* Image_hpp */
