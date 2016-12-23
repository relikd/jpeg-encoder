#include "YCbCrToRGBConverter.hpp"

void YCbCrToRGBConverter::convert(std::shared_ptr<Image> image) {

	Channel *ch1 = image->channel1;
	Channel *ch2 = image->channel2;
	Channel *ch3 = image->channel3;
	
	if (ch1->numberOfPixel() != ch2->numberOfPixel() || ch1->numberOfPixel() != ch3->numberOfPixel()) {
		fputs("Cannot compute RGB with different channel sizes\n", stderr);
		return;
	}
	
	image->seekAllChannelToStart(ChannelSeekRead);
	
	size_t index = image->channel1->numberOfPixel();
	while (index--) {
		color &y = ch1->readNextValue();
		color &cb = ch2->readNextValue();
		color &cr = ch3->readNextValue();
		color r = normalize(y +    1.402f * (cr - 0.5f));
		color g = normalize(y - 0.344136f * (cb - 0.5f) - 0.714136f * (cr - 0.5f));
		color b = normalize(y +    1.772f * (cb - 0.5f));
		y = r;
		cb = g;
		cr = b;
	}
	image->colorSpace = ColorSpaceRGB;
}

color YCbCrToRGBConverter::normalize(float value) {
	if (value < 0) return 0;
	if (value > 1) return 1;
	return (color) value;
}
