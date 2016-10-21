#ifndef JPEG_ENCODER_YCBCRTORGBCONVERTER_H
#define JPEG_ENCODER_YCBCRTORGBCONVERTER_H

#include <memory>
#include "../model/Image.hpp"

struct YCbCrToRGBConverter {
	std::shared_ptr<Image> convert(std::shared_ptr<Image> originalImage);

private:
	size_t normalize(int value);
};

#endif //JPEG_ENCODER_YCBCRTORGBCONVERTER_H