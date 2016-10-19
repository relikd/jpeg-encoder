#ifndef PPMLoader_hpp
#define PPMLoader_hpp

#include <iostream>
#include <memory>
#include "Image.hpp"

struct PPMLoader {
	PPMLoader() {}

	std::shared_ptr<Image> load(const char *pathToImage);
};

#endif /* PPMLoader_hpp */