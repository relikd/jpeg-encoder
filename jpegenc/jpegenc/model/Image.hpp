#ifndef Image_hpp
#define Image_hpp

#include <stdlib.h>
#include <stdio.h>

struct Channel {
	size_t size;
	size_t *values;

	Channel(size_t size = 0) : size(size) {
		values = new size_t[size];
	}

	~Channel() {
		delete[] values;
	}
};

struct Image {
	size_t width, height;
	std::string colorSpace;
	Channel *channel1;
	Channel *channel2;
	Channel *channel3;

	Image(size_t width, size_t height) : width(width), height(height) {
		size_t channelSize = width * height;
		channel1 = new Channel(channelSize);
		channel2 = new Channel(channelSize);
		channel3 = new Channel(channelSize);
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

	void print() const;

	void setColorSpace(std::string colorSpace) {
		this->colorSpace = colorSpace;
	}
};

#endif /* Image_hpp */