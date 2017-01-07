#ifndef Arai_hpp
#define Arai_hpp

#include <stdio.h>

struct Arai
{
public:
    static void transform(float* &values, size_t image_width, size_t image_height);
	static void transformInlineTranspose(float* &values, size_t image_width, size_t image_height);
private:
	static void processRows(float* &values, size_t numberOfPixels = 64);
	static void processColumns(float* &values, size_t image_width = 8, size_t image_height = 8);
};

#endif /* Arai_hpp */
