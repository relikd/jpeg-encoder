#ifndef JPEG_ENCODER_RGBTOYCBCRCONVERTER_H
#define JPEG_ENCODER_RGBTOYCBCRCONVERTER_H

#include <memory>
#include "../model/Image.hpp"

struct RGBToYCbCrConverter {
	void convert(std::shared_ptr<Image> image);
};

#endif //JPEG_ENCODER_RGBTOYCBCRCONVERTER_H
