#ifndef PPMLoader_hpp
#define PPMLoader_hpp

#include <iostream>
#include <memory>
#include "../model/Image.hpp"

struct PPMLoader {
	PPMLoader() {}
	
	std::shared_ptr<Image> customLoad(const char *pathToImage);
	void write(const char *pathToImage, std::shared_ptr<Image> image);

private:
	color normalize(color colorValue, int originalMaxValue, int normalizedMaxValue);
};

#endif /* PPMLoader_hpp */
