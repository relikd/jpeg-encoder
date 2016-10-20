#ifndef PPMLoader_hpp
#define PPMLoader_hpp

#include <iostream>
#include <memory>
#include "../model/Image.hpp"

struct PPMLoader {
	PPMLoader() {}

	std::shared_ptr<Image> load(const char *pathToImage);

private:
	size_t normalize(size_t colorValue, int originalMaxValue, int normalizedMaxValue);
};

#endif /* PPMLoader_hpp */