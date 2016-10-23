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

void Image::print() {
	std::cout << "Color space: " << colorSpace;
	for (int i = 0; i < width * height; ++i) {
		if (i % width == 0) {
			std::cout << std::endl;
		}
		std::cout << channel1->getValue( size_t (i), numberOfPixels ) << "\t";
		std::cout << channel2->getValue( size_t (i), numberOfPixels ) << "\t";
		std::cout << channel3->getValue( size_t (i), numberOfPixels ) << "\t\t";
	}
	std::cout << std::endl << std::endl;
}