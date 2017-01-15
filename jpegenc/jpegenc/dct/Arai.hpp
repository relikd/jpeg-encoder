#ifndef Arai_hpp
#define Arai_hpp

#include <stdio.h>

struct Arai
{
public:
    static void transform(float* values, size_t image_width, size_t image_height);
	static void transformInlineTranspose(float* values, size_t image_width, size_t image_height);
};

#endif /* Arai_hpp */
