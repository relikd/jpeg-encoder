#include <memory>
#include "PPMLoader.hpp"

std::shared_ptr<Image> PPMLoader::load(const char *pathToImage) {
	FILE *file = fopen(pathToImage, "r");

	char magicNumber[8];
	fscanf(file, "%s\n", magicNumber);

	int width, height;
	fscanf(file, "%d %d\n", &width, &height);

	int maxValue;
	fscanf(file, "%d\n", &maxValue);

	auto image = std::make_shared<Image>(width, height);

	size_t index = 0;

	while (1) {
		uint8_t r, g, b;
		int elementsRead = fscanf(file, "%d %d %d", &r, &g, &b);

		if (elementsRead < 3) {
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
