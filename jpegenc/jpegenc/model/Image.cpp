#include <iostream>
#include "Image.hpp"

std::ostream& operator<<(std::ostream& out, const color value){
	return out << (size_t)value;
}

std::ostream& operator<<(std::ostream& out, const ColorSpace value){
	switch (value) {
		case ColorSpaceRGB:   return out << "RGB";
		case ColorSpaceYCbCr: return out << "YCbCr";
	};
}

void Image::print() {
	std::cout << "Color space: " << colorSpace;
	size_t numberOfPixels = imageSize.pixelCount;
	for (size_t i = 0; i < numberOfPixels; ++i) {
		if (i % imageSize.width == 0) {
			std::cout << std::endl;
		}
		std::cout << channel1->getValue( i, imageSize ) << "\t";
		std::cout << channel2->getValue( i, imageSize ) << "\t";
		std::cout << channel3->getValue( i, imageSize ) << "\t\t";
	}
	std::cout << std::endl << std::endl;
}

void Image::reduceBySubSample(size_t xdiv, size_t ydiv) {
	channel1->reduceBySubSampling(xdiv, ydiv);
	channel2->reduceBySubSampling(xdiv, ydiv);
	channel3->reduceBySubSampling(xdiv, ydiv);
	imageSize /= Dimension(xdiv,ydiv);
}

void Image::reduceByAverage(size_t xdiv, size_t ydiv) {
	channel1->reduceByAveraging(xdiv, ydiv);
	channel2->reduceByAveraging(xdiv, ydiv);
	channel3->reduceByAveraging(xdiv, ydiv);
	imageSize /= Dimension(xdiv,ydiv);
}
