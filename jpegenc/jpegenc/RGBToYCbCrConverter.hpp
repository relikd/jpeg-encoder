#ifndef JPEG_ENCODER_RGBTOYCBCRCONVERTER_H
#define JPEG_ENCODER_RGBTOYCBCRCONVERTER_H

#include <memory>
#include "Image.hpp"

struct RGBToYCbCrConverter {
	std::shared_ptr<Image> convert(std::shared_ptr<Image> img);
};

#endif //JPEG_ENCODER_RGBTOYCBCRCONVERTER_H