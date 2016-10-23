#include "YCbCrToRGBConverter.h"

std::shared_ptr<Image> YCbCrToRGBConverter::convert(std::shared_ptr<Image> originalImage) {
	auto convertedImage = std::make_shared<Image>(originalImage->width, originalImage->height);
	size_t index = originalImage->width * originalImage->height;

	while (index) {
		--index;
		int y = (int) originalImage->getValueFromChannel1(index);
		int cb = (int) originalImage->getValueFromChannel2(index);
		int cr = (int) originalImage->getValueFromChannel3(index);
		int r = (int) (y + 1.4021 * (cr - 128));
		int g = (int) (y - 0.3441 * (cb - 128) - 0.7142 * (cr - 128));
		int b = (int) (y + 1.772 * (cb - 128));
		convertedImage->channel1->setValue(index, normalize(r));
		convertedImage->channel2->setValue(index, normalize(g));
		convertedImage->channel3->setValue(index, normalize(b));
	}
	convertedImage->colorSpace = ColorSpaceRGB;
	return convertedImage;
}

size_t YCbCrToRGBConverter::normalize(int value) {
	if (value < 0) {
		value = 0;
	}
	if (value > 255) {
		value = 255;
	}
	return (size_t) value;
}
