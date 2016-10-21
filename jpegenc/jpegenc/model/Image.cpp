#include <iostream>
#include "Image.hpp"

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

void Image::print() const {
	std::cout << "Color space: " << colorSpace;
	for (int i = 0; i < width * height; ++i) {
		if (i % width == 0) {
			std::cout << std::endl;
		}
		std::cout << channel1->values[i] << " " << channel2->values[i] << " " << channel3->values[i] << "	 ";
	}
	std::cout << std::endl << std::endl;
}