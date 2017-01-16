//
//  Quantization.hpp
//  jpegenc
//
//  Created by Marcel Groß on 10.01.17.
//  Copyright © 2017 FHWS. All rights reserved.
//

#ifndef Quantization_hpp
#define Quantization_hpp

#include <stdio.h>

class Quantization {
public:
	static void run(float* &dctCoefficient, size_t picWidth, size_t picHeigth, float* &quantizationMatrix);
};

#endif /* Quantization_hpp */
