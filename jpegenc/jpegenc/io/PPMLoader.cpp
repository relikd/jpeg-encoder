#include <fstream>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>

#include "PPMLoader.hpp"

std::shared_ptr<Image> PPMLoader::load(const char *pathToImage) {
	readFileToMemory(pathToImage);
	
	size_t bufferIndex = 2; // start right after the first two bytes 'P3'
	size_t width = 0, height = 0;
	unsigned int maxValue = 0; // uint because PPM maxVal is limited to 65535
	bool successfull = parseHeader(bufferIndex, width, height, maxValue);
	
	if (successfull) {
		auto image = std::make_shared<Image>(Dimension(width, height));
		image->colorSpace = ColorSpaceRGB;
		
		parseData(bufferIndex, image, maxValue);
		
		buffer = NULL; // not necessary, but anyway
		return image;
	}
	buffer = NULL; // not necessary, but anyway
	return nullptr;
}

void PPMLoader::write(const char *pathToImage, std::shared_ptr<Image> image) {

	if (image->colorSpace != ColorSpaceRGB) {
		std::cout << "Image color space is not RGB." << std::endl;
	}

	Dimension imageSize = image->imageSize;
	std::ofstream outputStream(pathToImage);
	size_t pixelCount = imageSize.pixelCount;

	outputStream << "P3" << "\n";
	outputStream << "# Created by Team Awesome" << "\n";
	outputStream << imageSize.width << " " << imageSize.height << "\n";
	outputStream << "255" << "\n";

	for (size_t i = 0; i < pixelCount; ++i) {
		outputStream << (size_t) image->channel1->getValue(i, imageSize) << " ";
		outputStream << (size_t) image->channel2->getValue(i, imageSize) << " ";
		outputStream << (size_t) image->channel3->getValue(i, imageSize) << " ";
		
		if ( (i + 1) % 4 == 0 ) {
			outputStream << "\n";
		}
	}
	outputStream << "n";
	outputStream.close();
}

// ################################################################
// #
// #  Helper Methods
// #
// ################################################################

color PPMLoader::normalize(color colorValue, const unsigned int originalMaxValue, const unsigned int normalizedMaxValue){
	if (originalMaxValue == normalizedMaxValue) {
		return colorValue;
	}
	return (color) ((colorValue / (float) originalMaxValue) * normalizedMaxValue);
}


// ################################################################
// #
// #  Read File
// #
// ################################################################

void PPMLoader::readFileToMemory(const char *pathToImage) {
	// fopen is used only to get the file size
	FILE *fl = fopen(pathToImage, "r");
	fseek(fl, 0, SEEK_END);
	filesize = ftell(fl);
	fclose(fl);
	
	// start actual file reading with mmap
	int fd = open(pathToImage, O_RDONLY);
	if (fd == -1) {
		perror("Error opening file for reading");
		exit(EXIT_FAILURE);
	}
	
	buffer = (char*)mmap(0, filesize, PROT_READ, MAP_SHARED, fd, 0);
	if (buffer == MAP_FAILED) {
		close(fd);
		perror("Error mmapping the file");
		exit(EXIT_FAILURE);
	}
	close(fd);
	/* the whole file is now loaded in the memory buffer. */
}

bool PPMLoader::parseHeader(size_t &index, size_t &width, size_t &height, unsigned int &maxValue) {
	// check if file is in the correct format
	if (buffer[0] != 'P' || buffer[1] != '3') {
		fputs("Invalid format. PPM P3 expected.\n", stderr);
		return false;
	}
	
	char c;
	int headerIndex = 0; // 0 = width, 1 = height, 2 = maxValue, 3 = first data value
	int headerValue = 0; // holds the actual number
	bool headerValueChanged = false;
	bool isComment = false;
	
	while (headerIndex < 4) {
		c = buffer[ index++ ];
		// skip comments till end of line
		if (isComment) {
			if (c == '\n')
				isComment = false;
			continue;
		}
		// calculate number
		if (c > 47 && c < 59) {
			if (headerIndex >2) {
				--index;
				return true; // index = begining of the first data value
			}
			headerValue = headerValue * 10 + c - 48;
			headerValueChanged = true;
			continue;
		}
		// save number to header info
		if (headerValueChanged) {
			switch ( headerIndex++ ) {
				case 0:    width = headerValue; break;
				case 1:   height = headerValue; break;
				case 2: maxValue = headerValue; break;
			}
			headerValue = 0;
			headerValueChanged = false;
		}
		
		if (c == '#')
			isComment = true;
	}
	return true;
}

void PPMLoader::parseData(size_t &index, std::shared_ptr<Image> image, const unsigned int maxValue) {
	char c;
	size_t pixelIndex = 0; // logical index to channel array index
	int value = 0; // holds the actual number
	bool valueChanged = false;
	
	Channel *channels[] = {image->channel1, image->channel2, image->channel3};
	char channelIndex = 0; // constantly switch the three channels
	
	while (index < filesize) {
		c = buffer[ index++ ];
		
		// calculate number
		if (c > 47 && c < 59) {
			value = value * 10 + c - 48;
			valueChanged = true;
			continue;
		}
		// save number to channel array
		if (valueChanged) {
			
			channels[channelIndex]->setValue(pixelIndex, normalize(value, maxValue, 255));
			
			channelIndex = (channelIndex + 1) % 3;
			if (channelIndex == 0)
				++pixelIndex;
			
			value = 0;
			valueChanged = false;
		}
	}
	
	// if there's no whitespace after the last number! Otherwise the loop will omit it
	if (value > 0)
		channels[channelIndex]->setValue(pixelIndex, normalize(value, maxValue, 255));
}

