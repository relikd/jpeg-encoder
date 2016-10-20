#include "RGBToYCbCrConverter.hpp"

std::shared_ptr<Image> RGBToYCbCrConverter::convert(std::shared_ptr<Image> originalImage) {
	auto convertedImage = std::make_shared<Image>(originalImage->width, originalImage->height);
	size_t index = originalImage->width * originalImage->height;

	while (index) {
		index--;
		size_t r = originalImage->getValueFromChannel1(index);
		size_t g = originalImage->getValueFromChannel2(index);
		size_t b = originalImage->getValueFromChannel3(index);
		size_t y = (size_t) (0.299 * r + 0.587 * g + 0.114 * b);
		size_t cb = (size_t) (-0.1687 * r - 0.3312 * g + 0.5 * b + 128);
		size_t cr = (size_t) (0.5 * r - 0.4186 * g - 0.0813 * b + 128);
		convertedImage->setValueOnChannel1(index, y);
		convertedImage->setValueOnChannel2(index, cb);
		convertedImage->setValueOnChannel3(index, cr);
	}
	convertedImage->setColorSpace("YCbCr");
	return convertedImage;
}