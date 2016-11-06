#include <iostream>
#include <math.h>
#include "Channel.hpp"

color Channel::getValue(size_t x, size_t y, Dimension size) {
	if (x >= size.width)
		x = size.width - 1;
	if (y >= size.height)
		y = size.height - 1;
	
	size_t index = x + size.width * y;
	size_t numberOfPixels = x * y;
	return getValue(index, numberOfPixels);
}

color Channel::getValue(size_t index, size_t numberOfPixels) {
	return values[ (index * imageSize.pixelCount) / numberOfPixels];
}

void Channel::setValue(size_t x, size_t y, Dimension size, color value) {
	if (x >= size.width || y >= size.height) {
		std::cout << "Cant set pixel out of range " << x << " " << y << std::endl;
		return;
	}
	size_t index = x + size.width * y;
	setValue(index, value);
}

void Channel::setValue(size_t index, color value) {
	values[index] = value;
}

void Channel::reduceBySubSampling(size_t stepWidth, size_t stepHeight) {
	size_t prevWidth = imageSize.width;
	reduceWithFunction(stepWidth, stepHeight, [&](size_t row, size_t col) {
		return values[ col*stepWidth + row*stepHeight*prevWidth ];
	});
}

void Channel::reduceByAveraging(size_t stepWidth, size_t stepHeight) {
	size_t prevWidth = imageSize.width;
	reduceWithFunction(stepWidth, stepHeight, [&](size_t row, size_t col) {
		size_t sum = 0;
		size_t pxCount = stepWidth * stepHeight;
		
		if (pxCount == 0)
			return values[ col*stepWidth + row*stepHeight*prevWidth ];
		
		size_t old_row = row * stepHeight;
		size_t old_col = col * stepWidth;
		for (int row_offset=0; row_offset < stepHeight; row_offset++) {
			for (int col_offset=0; col_offset < stepWidth; col_offset++) {
				sum += values[ old_col+col_offset + (old_row+row_offset)*prevWidth ];
			}
		}
		return (color) round(sum / (float)pxCount);
	});
}

void Channel::reduceWithFunction(size_t stepWidth, size_t stepHeight, std::function<color(size_t, size_t)> f) {
	imageSize /= Dimension(stepWidth, stepHeight);
	
	size_t newByteCount = imageSize.pixelCount;
	color *reducedValues = new color[newByteCount];
	
	size_t row, col;
	for (size_t i = 0; i < newByteCount; i++) {
		row = i/imageSize.width;
		col = i%imageSize.width;
		color calculatedValue = f(row, col);
		reducedValues[i] = calculatedValue;
	}
	values = reducedValues;
}
