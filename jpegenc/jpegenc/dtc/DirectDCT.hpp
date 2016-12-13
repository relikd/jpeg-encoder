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

class DirectDCT {
public:
	Mat transform(Mat input);
private:
	float getC(int i);
};
#endif /* directDCT_hpp */
