#include "RGBToYCbCrConverter.hpp"

void RGBToYCbCrConverter::convert(std::shared_ptr<Image> image) {
	
	Channel *ch1 = image->channel1;
	Channel *ch2 = image->channel2;
	Channel *ch3 = image->channel3;
	
	if (ch1->numberOfPixel() != ch2->numberOfPixel() || ch1->numberOfPixel() != ch3->numberOfPixel()) {
		fputs("Cannot compute YCbCr with different channel sizes\n", stderr);
		return;
	}
	
	color r, g, b;
	color y, cb, cr;
	Dimension size = image->imageSize;
	size_t index = size.pixelCount;
	
	while (index--) {
		r = ch1->getValue(index, size);
		g = ch2->getValue(index, size);
		b = ch3->getValue(index, size);
		y  = (color)     (0.299f * r +    0.587f * g +    0.114f * b);
		cb = (color) (-0.168736f * r - 0.331264f * g +      0.5f * b + 0.5f); // 0.5 == +128
		cr = (color)       (0.5f * r - 0.418688f * g - 0.081312f * b + 0.5f); // 0.5 == +128
		ch1->setValue(index, y);
		ch2->setValue(index, cb);
		ch3->setValue(index, cr);
	}
	image->colorSpace = ColorSpaceYCbCr;
}
