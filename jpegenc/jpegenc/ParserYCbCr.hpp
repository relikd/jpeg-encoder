#ifndef ParserYCbCr_hpp
#define ParserYCbCr_hpp

#include <iostream>
#include "Image.hpp"

class ParserYCbCr {
	
	
public:
	static std::shared_ptr<Image> parse(std::shared_ptr<Image> img);
	static Pixel toYCbCr(Pixel rgb);
};

#endif /* ParserYCbCr_hpp */
