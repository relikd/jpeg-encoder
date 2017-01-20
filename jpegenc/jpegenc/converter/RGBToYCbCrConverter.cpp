#include "RGBToYCbCrConverter.hpp"

void RGBToYCbCrConverter::convert(std::shared_ptr<Image> image) {
	
	Channel *ch1 = image->channel1;
	Channel *ch2 = image->channel2;
	Channel *ch3 = image->channel3;
	
	if (ch1->numberOfPixel() != ch2->numberOfPixel() || ch1->numberOfPixel() != ch3->numberOfPixel()) {
		fputs("Cannot compute YCbCr with different channel sizes\n", stderr);
		return;
	}
	
	image->seekAllChannelToStart(ChannelSeekRead);
	
	size_t index = image->channel1->numberOfPixel();
	while (index--) {
		color &r = ch1->readNextValue();
		color &g = ch2->readNextValue();
		color &b = ch3->readNextValue();
		color y  = (color)     (0.299f * r +    0.587f * g +    0.114f * b) - 0.5f;
		color cb = (color) (-0.168736f * r - 0.331264f * g +      0.5f * b + 0.5f) - 0.5f;
		color cr = (color)       (0.5f * r - 0.418688f * g - 0.081312f * b + 0.5f) - 0.5f;
		r = y;
		g = cb;
		b = cr;
	}
	image->colorSpace = ColorSpaceYCbCr;
}
