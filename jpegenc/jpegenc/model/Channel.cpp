#include "Channel.hpp"

color Channel::getValue(size_t x, size_t y, Dimension mapped_size) {
	if (x >= mapped_size.width)
		x = mapped_size.width - 1;
	if (y >= mapped_size.height)
		y = mapped_size.height - 1;
	
	if (imageSize != mapped_size) {
		x *= (imageSize.width / (float)mapped_size.width);
		y *= (imageSize.height / (float)mapped_size.height);
	}
	return values[ x + y*imageSize.width ];
}

color Channel::getValue(size_t index, Dimension mapped_size) {
	if (imageSize == mapped_size)
		return values[index];
	return getValue(index % mapped_size.width, index / mapped_size.width, mapped_size);
}

void Channel::setValue(size_t x, size_t y, color &value) {
	if (x >= imageSize.width || y >= imageSize.height) {
		printf("Cant set pixel out of range x:%lu y:%lu \n", x, y);
		return;
	}
	size_t index = x + imageSize.width * y;
	setValue(index, value);
}

void Channel::setValue(size_t index, color &value) {
	values[index] = value;
}

void Channel::reduceBySubSampling(size_t stepWidth, size_t stepHeight) {
	size_t prevWidth = imageSize.width;
	reduceWithFunction(stepWidth, stepHeight, [&](size_t row, size_t col) {
		return values[col * stepWidth + row * stepHeight * prevWidth];
	});
}

void Channel::reduceByAveraging(size_t stepWidth, size_t stepHeight) {
	size_t prevWidth = imageSize.width;
	short row_offset = stepHeight;
	short col_offset = stepWidth;
	
	reduceWithFunction(stepWidth, stepHeight, [&](size_t row, size_t col) {
		float sum = 0;
		size_t pxCount = stepWidth * stepHeight;
		
		if (pxCount == 0) // then sub sampling
			return values[col * stepWidth + row * stepHeight * prevWidth];
		
		size_t old_row = row * stepHeight;
		size_t old_col = col * stepWidth;
		row_offset = stepHeight;
		
		while (row_offset--) {
			while (col_offset--) { // Average
				sum += values[old_col + col_offset + (old_row + row_offset) * prevWidth];
			}
			col_offset = stepWidth;
		}
		return (color)(sum / pxCount);
	});
}

void Channel::reduceWithFunction(size_t stepWidth, size_t stepHeight, std::function<color(size_t, size_t)> f) {
	imageSize /= Dimension(stepWidth, stepHeight);
	
	size_t newByteCount = imageSize.pixelCount;
	color *reducedValues = new color[newByteCount];
	
	size_t row, col;
	for (size_t i = 0; i < newByteCount; i++) {
		row = i / imageSize.width;
		col = i % imageSize.width;
		reducedValues[i] = f(row, col);
	}
	values = reducedValues;
}
