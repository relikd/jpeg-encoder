#include <iostream>
#include "Image.hpp"

size_t Channel::getValue(size_t x, size_t y, size_t width, size_t height) {
	if (x >= width) {
		x = width - 1;
	}
	if (y >= height) {
		y = height - 1;
	}
	size_t index = x + width * y;
	size_t numberOfPixels = x * y;
	return getValue(index, numberOfPixels);
}

size_t Channel::getValue(size_t index, size_t numberOfPixels) {
	return values[(index * channelSize) / numberOfPixels];
}

void Channel::setValue(size_t x, size_t y, size_t width, size_t height, size_t value) {
	if (x >= width || y >= height) {
		std::cout << "Cant set pixel out of range " << x << " " << y << std::endl;
		return;
	}
	size_t index = x + width * y;
	setValue(index, value);
}

void Channel::setValue(size_t index, size_t value) {
	values[index] = value;
}

void Channel::reduceBySubSampling(size_t stepWidth) {
	channelSize = channelSize / stepWidth;
	size_t *reducedValues = new size_t[channelSize];

	for ( int i = 0; i < channelSize; ++i) {
		reducedValues[i] = values[i*stepWidth];
	}
	values = reducedValues;
}

void Channel::reduceByAveraging(size_t stepWidth) {
	channelSize = channelSize / stepWidth;
	size_t *reducedValues = new size_t[channelSize];

	for ( int i = 0; i < channelSize; ++i) {
		size_t sum = 0;

		for ( int k = 0; k < stepWidth; k++ )
		{
			sum = sum + values[i*stepWidth + k];
		}
		reducedValues[i] = (size_t) sum / stepWidth;
	}
	values = reducedValues;
}

size_t Image::getIndex(size_t x, size_t y) const {
	return x + width * y;
};

size_t Image::getValueFromChannel1(size_t x, size_t y) {
	if (x >= width) {
		x = width - 1;
	}
	if (y >= height) {
		y = height - 1;
	}
	return getValueFromChannel1(getIndex(x, y));
}

size_t Image::getValueFromChannel2(size_t x, size_t y) {
	if (x >= width) {
		x = width - 1;
	}
	if (y >= height) {
		y = height - 1;
	}
	return getValueFromChannel2(getIndex(x, y));
}

size_t Image::getValueFromChannel3(size_t x, size_t y) {
	if (x >= width) {
		x = width - 1;
	}
	if (y >= height) {
		y = height - 1;
	}
	return getValueFromChannel3(getIndex(x, y));
}

size_t Image::getValueFromChannel1(size_t index) {
	return channel1->values[(index * channel1->channelSize) / numberOfPixels];
}

size_t Image::getValueFromChannel2(size_t index) {
	return channel2->values[(index * channel2->channelSize) / numberOfPixels];
}

size_t Image::getValueFromChannel3(size_t index) {
	return channel3->values[(index * channel3->channelSize) / numberOfPixels];
}

void Image::setValueOnChannel1(size_t x, size_t y, size_t value) {
	if (x >= width || y >= height) {
		std::cout << "Cant set pixel out of range " << x << " " << y << std::endl;
		return;
	}
	setValueOnChannel1(getIndex(x, y), value);
}

void Image::setValueOnChannel2(size_t x, size_t y, size_t value) {
	if (x >= width || y >= height) {
		std::cout << "Cant set pixel out of range " << x << " " << y << std::endl;
		return;
	}
	setValueOnChannel2(getIndex(x, y), value);
}

void Image::setValueOnChannel3(size_t x, size_t y, size_t value) {
	if (x >= width || y >= height) {
		std::cout << "Cant set pixel out of range " << x << " " << y << std::endl;
		return;
	}
	setValueOnChannel3(getIndex(x, y), value);
}

void Image::setValueOnChannel1(size_t index, size_t value) {
	channel1->values[index] = value;
}

void Image::setValueOnChannel2(size_t index, size_t value) {
	channel2->values[index] = value;
}

void Image::setValueOnChannel3(size_t index, size_t value) {
	channel3->values[index] = value;
}

void Image::print() {
	std::cout << "Color space: " << colorSpace;
	for (int i = 0; i < width * height; ++i) {
		if (i % width == 0) {
			std::cout << std::endl;
		}
		std::cout << getValueFromChannel1( size_t (i) ) << "\t";
		std::cout << getValueFromChannel2( size_t (i) ) << "\t";
		std::cout << getValueFromChannel3( size_t (i) ) << "\t\t";
	}
	std::cout << std::endl << std::endl;
}

void Image::reduceBySubSamplingChannel1(size_t stepWidth) {
	Channel *reducedChannel = new Channel(channel1->channelSize/stepWidth);

	for ( int i = 0; i < reducedChannel->channelSize; ++i) {
		reducedChannel->values[i] = channel1->values[i*stepWidth];
	}
	*channel1 = *reducedChannel;
}

void Image::reduceBySubSamplingChannel2(size_t stepWidth) {
	Channel *reducedChannel = new Channel(channel2->channelSize/stepWidth);

	for ( int i = 0; i < reducedChannel->channelSize; ++i) {
		reducedChannel->values[i] = channel2->values[i*stepWidth];
	}
	*channel2 = *reducedChannel;
}

void Image::reduceBySubSamplingChannel3(size_t stepWidth) {
	Channel *reducedChannel = new Channel(channel3->channelSize/stepWidth);

	for ( int i = 0; i < reducedChannel->channelSize; ++i) {
		reducedChannel->values[i] = channel3->values[i*stepWidth];
	}
	*channel3 = *reducedChannel;
}

void Image::reduceByAveragingChannel1(size_t stepWidth) {
	Channel *reducedChannel = new Channel(channel1->channelSize/stepWidth);

	for ( int i = 0; i < reducedChannel->channelSize; ++i) {
		size_t sum = 0;

		for ( int k = 0; k < stepWidth; k++ )
		{
			sum = sum + channel1->values[i*stepWidth + k];
		}
		reducedChannel->values[i] = (size_t) sum / stepWidth;
	}
	*channel1 = *reducedChannel;
}

void Image::reduceByAveragingChannel2(size_t stepWidth) {
	Channel *reducedChannel = new Channel(channel2->channelSize/stepWidth);

	for ( int i = 0; i < reducedChannel->channelSize; ++i) {
		size_t sum = 0;

		for ( int k = 0; k < stepWidth; k++ )
		{
			sum = sum + channel2->values[i * stepWidth + k];
		}
		reducedChannel->values[i] = (size_t) sum / stepWidth;
	}
	*channel2 = *reducedChannel;
}

void Image::reduceByAveragingChannel3(size_t stepWidth) {
	Channel *reducedChannel = new Channel(channel3->channelSize/stepWidth);

	for ( int i = 0; i < reducedChannel->channelSize; ++i) {
		size_t sum = 0;

		for ( int k = 0; k < stepWidth; k++ )
		{
			sum = sum + channel3->values[i * stepWidth + k];
		}
		reducedChannel->values[i] = (size_t) sum / stepWidth;
	}
	*channel3 = *reducedChannel;
}