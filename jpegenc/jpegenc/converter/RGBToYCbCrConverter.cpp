#include "RGBToYCbCrConverter.hpp"

std::shared_ptr<Image> RGBToYCbCrConverter::convert(std::shared_ptr<Image> originalImage) {
	auto convertedImage = std::make_shared<Image>(originalImage->width, originalImage->height);
	size_t index = originalImage->width * originalImage->height;

	while (index) {
		--index;
		size_t r = originalImage->channel1->getValue(index, originalImage->numberOfPixels );
		size_t g = originalImage->channel2->getValue(index, originalImage->numberOfPixels );
		size_t b = originalImage->channel3->getValue(index, originalImage->numberOfPixels );
		size_t y = (size_t) (0.299 * r + 0.587 * g + 0.114 * b);
		size_t cb = (size_t) (-0.1687 * r - 0.3312 * g + 0.5 * b + 128);
		size_t cr = (size_t) (0.5 * r - 0.4186 * g - 0.0813 * b + 128);
		convertedImage->channel1->setValue(index, y);
		convertedImage->channel2->setValue(index, cb);
		convertedImage->channel3->setValue(index, cr);
	}
	convertedImage->colorSpace = ColorSpaceYCbCr;
	return convertedImage;
}
