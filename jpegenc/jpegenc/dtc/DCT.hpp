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
	static Mat transform(Mat input);
	static Mat transform2(Mat input);
	static Mat inverse(Mat input);
private:
	static float getC(int i);
	static Mat generateA(int dimension);
};


#endif /* directDCT_hpp */
