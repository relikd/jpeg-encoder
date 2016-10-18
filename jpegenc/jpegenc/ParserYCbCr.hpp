//
//  ParserYCbCr.hpp
//  jpegenc
//
//  Created by Oleg on 18/10/16.
//  Copyright © 2016 FHWS. All rights reserved.
//

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
