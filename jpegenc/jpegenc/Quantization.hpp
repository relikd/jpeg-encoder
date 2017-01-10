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
	static void run(float* &dctCoefficient,float* &quantizationMatrix);
};

#endif /* Quantization_hpp */
