#include <fstream>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>

#include "PPMLoader.hpp"

std::shared_ptr<Image> PPMLoader::load(const char *pathToImage) {
	
	// just to get the file size
	FILE *fl = fopen(pathToImage, "r");
	fseek(fl, 0, SEEK_END);
	long lSize = ftell(fl);
	fclose(fl);
	
	// start actual file reading with mmap
	int fd;
	char *buffer;
	
	fd = open(pathToImage, O_RDONLY);
	if (fd == -1) {
		perror("Error opening file for reading");
		exit(EXIT_FAILURE);
	}
	
	buffer = (char*)mmap(0, lSize, PROT_READ, MAP_SHARED, fd, 0);
	if (buffer == MAP_FAILED) {
		close(fd);
		perror("Error mmapping the file");
		exit(EXIT_FAILURE);
	}
	close(fd);
	
//	FILE *file = fopen(pathToImage, "r");
//	if (file == NULL) {
//		fputs("File error", stderr);
//		exit(1);
//	}
//
//	fseek(file, 0, SEEK_END);
//	long lSize = ftell(file);
//	rewind(file);
//
//	// allocate memory to contain the whole file:
//	char *buffer = new char[lSize];
//	if (buffer == NULL) {
//		fputs("Memory error", stderr);
//		exit(2);
//	}
//
//	// copy the file into the buffer:
//	size_t result = fread(buffer, 1, lSize, file);
//	if (result != lSize) {
//		fputs("Reading error", stderr);
//		exit(3);
//	}

	/* the whole file is now loaded in the memory buffer. */

	bool isComment = false;
	bool skipNextWhitespace = true;
	int step = 0;
	int width = 0, height = 0, maxValue = 0;

	size_t lastPos = 0;

	for (size_t i = 0; i < lSize; i++) {
		char c = buffer[i];
		if (c == '#')
			isComment = true;
		else if (isComment == false) {
			if (skipNextWhitespace == false && (c == ' ' || c == '\t' || c == '\n')) {
				step++;

				if (step == 4) {
					lastPos = i + 1;
					break;
				}

				skipNextWhitespace = true;
				continue;
			}

			bool changed = true;
			if (step == 0 && c == 'P') // read Magic Number
				if (buffer[i + 1] != '3') {
					fputs("Invalid format. PPM P3 expected.\n", stderr);
					exit(4);
				} else {}
			else if (step == 1 && c > 47 && c < 59) // width
				width = width * 10 + c - 48;
			else if (step == 2 && c > 47 && c < 59) // height
				height = height * 10 + c - 48;
			else if (step == 3 && c > 47 && c < 59) // max Value
				maxValue = maxValue * 10 + c - 48;
			else
				changed = false;

			if (changed)
				skipNextWhitespace = false;
		} else if (c == '\n')
			isComment = false;
	}


	auto image = std::make_shared<Image>(Dimension(width, height));
	size_t index = 0;
	size_t singleValue = 0;
	Channel *channels[] = {image->channel1, image->channel2, image->channel3};
	char cSel = 0;

	skipNextWhitespace = true;

	for (size_t i = lastPos; i < lSize; i++) {
		char c = buffer[i];
		if (c == '#')
			isComment = true;
		else if (isComment == false) {
			if (skipNextWhitespace == false && (c == ' ' || c == '\t' || c == '\n')) {

				channels[cSel]->setValue(index, normalize(singleValue, maxValue, 255));

				cSel = (cSel + 1) % 3;
				if (cSel == 0)
					index++;
				singleValue = 0;
				skipNextWhitespace = true;
				continue;
			}

			if (c > 47 && c < 59) {
				singleValue = singleValue * 10 + c - 48;
				skipNextWhitespace = false;
			}


		} else if (c == '\n')
			isComment = false;
	}

	if (singleValue > 0)
		channels[cSel]->setValue(index, normalize(singleValue, maxValue, 255));


	// terminate
//	fclose(file);
//	delete[] buffer;

	image->colorSpace = ColorSpaceRGB;
	return image;

}

color PPMLoader::normalize(color colorValue, int originalMaxValue, int normalizedMaxValue){
	if (originalMaxValue == normalizedMaxValue) {
		return colorValue;
	}
	return (color) ((colorValue / (float) originalMaxValue) * normalizedMaxValue);
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
