#ifndef Arai_hpp
#define Arai_hpp

#include <stdio.h>

struct Arai
{
public:
    static void transform(float* values, const size_t image_width, const size_t image_height);
	static void transformInlineTranspose(float* values, const size_t image_width, const size_t image_height);
	
	static void transformMT(float* values, const size_t image_width, const size_t image_height);
	static void transformInlineTransposeMT(float* values, const size_t image_width, const size_t image_height);
};

#endif /* Arai_hpp */
