#include "YCbCrToRGBConverter.hpp"

void YCbCrToRGBConverter::convert(std::shared_ptr<Image> image) {
	
	Channel *ch1 = image->channel1;
	Channel *ch2 = image->channel2;
	Channel *ch3 = image->channel3;
	
	if (ch1->numberOfPixel() != ch2->numberOfPixel() || ch1->numberOfPixel() != ch3->numberOfPixel()) {
		fputs("Cannot compute RGB with different channel sizes\n", stderr);
		return;
	}
	
	color r, g, b;
	color y, cb, cr;
	Dimension size = image->imageSize;
	size_t index = size.pixelCount;
	
	while (index--) {
		y  = ch1->getValue(index, size);
		cb = ch2->getValue(index, size) - 0.5f;
		cr = ch3->getValue(index, size) - 0.5f;
		r = normalize(y +    1.402f * cr);
		g = normalize(y - 0.344136f * cb - 0.714136f * cr);
		b = normalize(y +    1.772f * cb);
		ch1->setValue(index, r);
		ch2->setValue(index, g);
		ch3->setValue(index, b);
	}
	image->colorSpace = ColorSpaceRGB;
}

color YCbCrToRGBConverter::normalize(float value) {
	if (value < 0) return 0;
	if (value > 1) return 1;
	return (color) value;
}
