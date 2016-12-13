//
//  IDCT.hpp
//  jpegenc
//
//  Created by Marcel Groß on 13.12.16.
//  Copyright © 2016 FHWS. All rights reserved.
//

#ifndef IDCT_hpp
#define IDCT_hpp

#include <stdio.h>
#include "Mat.hpp"

class IDCT {
public:
	Mat transform(Mat input);
private:
	float getC(int i);
};

#endif /* IDCT_hpp */
