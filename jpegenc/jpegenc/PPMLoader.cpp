#include <memory>
#include "PPMLoader.hpp"

std::shared_ptr<Image> PPMLoader::load(const char *pathToImage) {
	FILE *file = fopen(pathToImage, "r");

	char magicNumber[255];
	fscanf(file, "%s\n", magicNumber);

	int width, height;
	fscanf(file, "%d %d\n", &width, &height);

	int maxValue;
	fscanf(file, "%d\n", &maxValue);

	auto image = std::make_shared<Image>(width, height);

	size_t index = 0;

	while (1) {
		int r, g, b;
		int elementsRead = fscanf(file, "%d %d %d", &r, &g, &b);

		if (elementsRead < 3) {
			break;
		}
		image->setPixel(index++, Pixel(r, g, b).set_highest_to_255_from(maxValue));
	}
	return image;
}
