#ifndef JPEG_ENCODER_YCBCRTORGBCONVERTER_H
#define JPEG_ENCODER_YCBCRTORGBCONVERTER_H

#include <memory>
#include "../model/Image.hpp"

struct YCbCrToRGBConverter {
	void convert(std::shared_ptr<Image> image);

private:
	color normalize(float value);
};

#endif //JPEG_ENCODER_YCBCRTORGBCONVERTER_H
