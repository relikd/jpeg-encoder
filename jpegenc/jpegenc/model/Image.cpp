#include <iostream>
#include "Image.hpp"


std::ostream& operator<<(std::ostream& out, const ColorSpace value){
	switch (value) {
		case ColorSpaceRGB:   return out << "RGB";
		case ColorSpaceYCbCr: return out << "YCbCr";
	};
}

void Image::print(bool useIntValues) {
	std::cout << "Color space: " << colorSpace << std::endl;
	
	setReadingRuleForAllChannel();
	
	size_t numberOfPixels = imageSize.pixelCount;
	for (size_t i = 0; i < numberOfPixels; ++i) {
		
		if (useIntValues) {
			std::cout << (int)(channel1->readNextValue()*255 + 0.5f) << "\t";
			std::cout << (int)(channel2->readNextValue()*255 + 0.5f) << "\t";
			std::cout << (int)(channel3->readNextValue()*255 + 0.5f) << "\t\t";
		} else {
			std::cout << channel1->readNextValue() << "\t";
			std::cout << channel2->readNextValue() << "\t";
			std::cout << channel3->readNextValue() << "\t\t";
		}
		
		if (i % imageSize.width == imageSize.width - 1) {
			std::cout << std::endl;
			size_t removeAdditionalColumns = channel1->imageSize.width - imageSize.width;
			while (removeAdditionalColumns--) {
				channel1->readNextValue(); // skip if image doesnt fit block size
				channel2->readNextValue();
				channel3->readNextValue();
			}
		}
	}
	std::cout << std::endl;
}

void Image::reduceBySubSample(unsigned short xdiv, unsigned short ydiv) {
	channel1->reduceBySubSampling(xdiv, ydiv);
	channel2->reduceBySubSampling(xdiv, ydiv);
	channel3->reduceBySubSampling(xdiv, ydiv);
	imageSize = Dimension(imageSize.width / xdiv, imageSize.height / ydiv);
}

void Image::reduceByAverage(unsigned short xdiv, unsigned short ydiv) {
	channel1->reduceByAveraging(xdiv, ydiv);
	channel2->reduceByAveraging(xdiv, ydiv);
	channel3->reduceByAveraging(xdiv, ydiv);
	imageSize = Dimension(imageSize.width / xdiv, imageSize.height / ydiv);
}

void Image::seekAllChannelToStart(ChannelSeekType type) {
	channel1->seekTo(type);
	channel2->seekTo(type);
	channel3->seekTo(type);
}

void Image::setReadingRuleForAllChannel() {
	channel1->setReadIndexAccessMappingRule(imageSize);
	channel2->setReadIndexAccessMappingRule(imageSize);
	channel3->setReadIndexAccessMappingRule(imageSize);
}

