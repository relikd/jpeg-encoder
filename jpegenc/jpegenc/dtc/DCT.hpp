//
//  directDCT.hpp
//  jpegenc
//
//  Created by Marcel Groß on 13.12.16.
//  Copyright © 2016 FHWS. All rights reserved.
//

#ifndef directDCT_hpp
#define directDCT_hpp

#include <stdio.h>
#include "Mat.hpp"

struct DCT {
	Mat transform(Mat input);
	Mat transform2DDCT(Mat input);
	Mat inverse(Mat input);
private:
	float getC(int i);
	Mat generateA(int dimension);
};


#endif /* directDCT_hpp */
