#ifndef PPMLoader_hpp
#define PPMLoader_hpp

#include <iostream>
#include <memory>
#include "../model/Image.hpp"

struct PPMLoader {
	PPMLoader() {}
	
	std::shared_ptr<Image> load(const char *pathToImage);
	std::shared_ptr<Image> customLoad(const char *pathToImage);

private:
	color normalize(color colorValue, int originalMaxValue, int normalizedMaxValue);
	int	scanForPattern(FILE * file, const char * fmt , void* arg0, void* arg1 = nullptr, void* arg2 = nullptr);
};

#endif /* PPMLoader_hpp */
