#include "RGBToYCbCrConverter.hpp"

std::shared_ptr<Image> RGBToYCbCrConverter::convert(std::shared_ptr<Image> originalImage) {
	
	auto convertedImage = std::make_shared<Image>(originalImage->imageSize);
	size_t originalImagePixelCount = originalImage->imageSize.pixelCount;
	size_t index = originalImagePixelCount;

	while (index) {
		--index;
		color r = originalImage->channel1->getValue(index, originalImage->imageSize );
		color g = originalImage->channel2->getValue(index, originalImage->imageSize );
		color b = originalImage->channel3->getValue(index, originalImage->imageSize );
		color y = (color) (0.299 * r + 0.587 * g + 0.114 * b);
		color cb = (color) (-0.1687 * r - 0.3312 * g + 0.5 * b + 128);
		color cr = (color) (0.5 * r - 0.4186 * g - 0.0813 * b + 128);
		convertedImage->channel1->setValue(index, y);
		convertedImage->channel2->setValue(index, cb);
		convertedImage->channel3->setValue(index, cr);
	}
	convertedImage->colorSpace = ColorSpaceYCbCr;
	return convertedImage;
}
