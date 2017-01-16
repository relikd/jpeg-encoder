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
#include <stdint.h>


class Quantization {
public:
	static void run(float* &dctCoefficient, size_t picWidth, size_t picHeigth, uint8_t* &quantizationMatrix);
    
    static const uint8_t* getLuminanceQT();
    static const uint8_t* getChrominanceQT();
};

#endif /* Quantization_hpp */
