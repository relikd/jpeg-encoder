#include "RGBToYCbCrConverter.hpp"
#include <math.h>

std::shared_ptr<Image> RGBToYCbCrConverter::convert(std::shared_ptr<Image> originalImage) {
	
	Dimension size = originalImage->imageSize;
	auto convertedImage = std::make_shared<Image>(size);
	
	size_t index = size.pixelCount;
	while (index) {
		--index;
		color r = originalImage->channel1->getValue(index, size);
		color g = originalImage->channel2->getValue(index, size);
		color b = originalImage->channel3->getValue(index, size);
		color y = (color) round((0.299 * r + 0.587 * g + 0.114 * b));
		color cb = (color) round((-0.1687 * r - 0.3312 * g + 0.5 * b + 128));
		color cr = (color) round((0.5 * r - 0.4186 * g - 0.0813 * b + 128));
		convertedImage->channel1->setValue(index, y);
		convertedImage->channel2->setValue(index, cb);
		convertedImage->channel3->setValue(index, cr);
	}
	convertedImage->colorSpace = ColorSpaceYCbCr;
	return convertedImage;
}
