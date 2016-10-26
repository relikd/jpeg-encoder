#include <memory>
#include "PPMLoader.hpp"

std::shared_ptr<Image> PPMLoader::load(const char *pathToImage) {
	FILE *file = fopen(pathToImage, "r");
	
	char magicNumber[8];
	scanForPattern(file, "%s\n", magicNumber);

	int width, height;
	scanForPattern(file, "%d %d\n", &width, &height);

	int maxValue;
	scanForPattern(file, "%d\n", &maxValue);

	auto image = std::make_shared<Image>(width, height);

	size_t index = 0;

	while (1) {
		uint r, g, b;
		int elementsRead = scanForPattern(file, "%d %d %d", &r, &g, &b);

		if (elementsRead  == EOF) {
			break;
		}
		image->channel1->setValue( index, normalize(r, maxValue, 255));
		image->channel2->setValue( index, normalize(g, maxValue, 255));
		image->channel3->setValue( index, normalize(b, maxValue, 255));
		++index;
	}
	image->colorSpace = ColorSpaceRGB;
	return image;
}

size_t PPMLoader::normalize(size_t colorValue, int originalMaxValue, int normalizedMaxValue) {
	return (size_t) ((colorValue / (float) originalMaxValue) * normalizedMaxValue);
}

int	PPMLoader::scanForPattern(FILE * file, const char * fmt , void* arg0, void* arg1, void* arg2) {
	int actualFound = 0;
	bool continueSearch = true;
	while(continueSearch) {
		char comment[255];
		continueSearch = fscanf(file, "# %99[^\n]", comment);
		
		if (!continueSearch) {
			if (arg2 != nullptr) {
				actualFound = fscanf(file, fmt, arg0, arg1, arg2);
			} else if (arg1 != nullptr) {
				actualFound = fscanf(file, fmt, arg0, arg1);
			} else {
				actualFound = fscanf(file, fmt, arg0);
			}
		}
	}
	
	return actualFound;
}


