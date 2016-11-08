#ifndef PPMLoader_hpp
#define PPMLoader_hpp

#include "../model/Image.hpp"

class PPMLoader {
public:
	PPMLoader() {}
	std::shared_ptr<Image> load(const char *pathToImage);
	void write(const char *pathToImage, std::shared_ptr<Image> image);

private:
	long filesize;
	char *buffer; // the whole file content
	
	color normalize(color colorValue, const unsigned int originalMaxValue, const unsigned int normalizedMaxValue);
	
	void readFileToMemory(const char *pathToImage);
	bool parseHeader(size_t &index, size_t &width, size_t &height, unsigned int &maxValue);
	void parseData(size_t &index, std::shared_ptr<Image> image, const unsigned int maxValue);
};

#endif /* PPMLoader_hpp */
