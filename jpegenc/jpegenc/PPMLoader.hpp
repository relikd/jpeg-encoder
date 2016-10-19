#ifndef PPMLoader_hpp
#define PPMLoader_hpp

#include <iostream>
#include "Image.hpp"
#include <memory>


struct PPMLoader {
	const char* file;
	
	PPMLoader(const char* file): file(file) {}
	
	std::shared_ptr<Image> load();
};
#endif /* PPMLoader_hpp */
