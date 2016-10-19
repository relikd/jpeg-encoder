//
//  ParserYCbCr.cpp
//  jpegenc
//
//  Created by Oleg on 18/10/16.
//  Copyright © 2016 FHWS. All rights reserved.
//

#include <memory>
#include "ParserYCbCr.hpp"

std::shared_ptr<Image> ParserYCbCr::parse(std::shared_ptr<Image> img) {
	auto image = std::make_shared<Image>(img->width, img->height);
	size_t idx = img->width * img->height;
	while (idx) {
		idx--;
		image->pixels[idx] = toYCbCr(img->pixels[idx]);
	}
	
	return image;
}

Pixel ParserYCbCr::toYCbCr(Pixel rgb) {
	Pixel ycc;
	ycc.r = 0.299*rgb.r + 0.587*rgb.g + 0.114*rgb.b;
	ycc.g = -0.1687*rgb.r -0.3312*rgb.g + 0.5*rgb.b  +128;
	ycc.b = 0.5*rgb.r -0.4186*rgb.g -0.0813*rgb.b    +128;
	return ycc;
}
