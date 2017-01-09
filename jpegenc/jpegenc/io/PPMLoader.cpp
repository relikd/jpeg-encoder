#include "PPMLoader.hpp"
#include "../helper/BitMath.hpp"

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
// Windows System
#else

#define USE_UNIX_MMAP_FILE 1
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>

#endif


std::shared_ptr<Image> PPMLoader::load(const char *pathToImage) {
	readFileToMemory(pathToImage);
	
	size_t bufferIndex = 2; // start right after the first two bytes 'P3'
	size_t width = 0, height = 0;
	unsigned short maxValue = 0; // short because PPM maxVal is limited to 65535
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
	size_t pixelCount = imageSize.pixelCount * 3; // 3 channel
	
	char out[4096];
	unsigned short idx = 0;
	
	FILE *f = fopen(pathToImage, "w");
	
	//  HEADER
	fwrite("P3\n# Created by Team Awesome\n", 1, 29, f);
	readNumberToFileSaveBuffer(imageSize.width, out, idx);
	readNumberToFileSaveBuffer(imageSize.height, out, idx);
	out[idx++] = '\n';
	readNumberToFileSaveBuffer(255, out, idx); // maxValue
	out[idx++] = '\n';
	
	
	Channel *channels[] = {image->channel1, image->channel2, image->channel3};
	image->setReadingRuleForAllChannel();
	
	unsigned short charactersOnLine = 0;
	for (size_t i = 0; i < pixelCount; i++) {
		
		unsigned int zahl = channels[i%3]->readNextValue() * 255;
		unsigned short stellen = BitMath::numberOfDigitsBase10(zahl);
		charactersOnLine += stellen + 1;
		
		// if the next number will exceed the 70 character limit, start new line
		if (charactersOnLine > 69) { // + 1 for ' '
			out[idx-1] = '\n'; // replace ' ' with '\n'
			charactersOnLine = stellen + 1; // put number on new line
			
			if (idx + 70 > 4095) { // once buffer full, write to disk
				fwrite(out, 1, idx, f);
				idx = 0;
			}
		}
		// parse int-number to string-number
		readNumberToFileSaveBuffer(zahl, stellen, out, idx);
		idx += stellen + 1;
	}
	
	// write all remaining bytes
	fwrite(out, 1, idx, f);
	fclose(f);
}

inline void PPMLoader::readNumberToFileSaveBuffer(size_t number, char *buf, unsigned short &index) {
	unsigned short stellen = BitMath::numberOfDigitsBase10(number);
	readNumberToFileSaveBuffer(number, stellen, buf, index);
	index += stellen + 1;
}

inline void PPMLoader::readNumberToFileSaveBuffer(size_t zahl, unsigned short stellen, char *buf, unsigned short index) {
	// write buffer last character first
	buf[index + stellen] = ' ';
	while (stellen--) {
		buf[index + stellen] = (zahl % 10) + 48; // 48 == '0'
		zahl /= 10;
	}
}

// ################################################################
// #
// #  Helper Methods
// #
// ################################################################

inline color normalize(const unsigned short &inputValue, const unsigned short &maxValue){
	return inputValue / (color) maxValue;
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
	
#ifndef USE_UNIX_MMAP_FILE // use the slower C fread()
	rewind(fl);
	
	// allocate memory to contain the whole file:
	buffer = new char[filesize];
	// copy the file into the buffer:
	size_t result = fread(buffer, 1, filesize, fl);
	if (result != filesize) {
		fputs("Reading error", stderr);
		exit(3);
	}
#endif
	
	fclose(fl);
	
#ifdef USE_UNIX_MMAP_FILE
	// blazing fast file to memory read with mmap
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
#endif
	/* the whole file is now loaded in the memory buffer. */
}

bool PPMLoader::parseHeader(size_t &index, size_t &width, size_t &height, unsigned short &maxValue) {
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
			if (headerIndex > 2) {
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

void PPMLoader::parseData(size_t &index, std::shared_ptr<Image> image, const unsigned short maxValue) {
	int value = 0; // holds the actual number
	bool valueChanged = false;
	
	size_t channelIndex = 0; // constantly switch the three channels
	Channel *channels[] = {image->channel1, image->channel2, image->channel3};
	image->seekAllChannelToStart(ChannelSeekWrite);
	
	while (index < filesize) {
		char c = buffer[ index++ ];
		
		// calculate number
		if (c > 47 && c < 59) {
			value = value * 10 + c - 48;
			valueChanged = true;
			continue;
		}
		// save number to channel array
		if (valueChanged) {
			channels[channelIndex]->appendValueAndFillupBlocks( normalize(value, maxValue) );
			
			if (++channelIndex > 2)
				channelIndex = 0;
			
			value = 0;
			valueChanged = false;
		}
	}
	
	// if there's no whitespace after the last number! Otherwise the loop will omit it
	if (value > 0) {
		channels[channelIndex]->appendValue( normalize(value, maxValue) );
	}
}

