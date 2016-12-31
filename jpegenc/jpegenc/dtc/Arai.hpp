#ifndef Arai_hpp
#define Arai_hpp

#include <stdio.h>
#include "Mat.hpp"

struct Arai
{
public:
    static Mat transform(Mat matrix);
    static void transformLine(float **x);
	
	static void transformMT(float* values);
	static void transformOG(float* &values, size_t image_width, size_t image_height);
	static void transformInlineTranspose(float* &values, size_t image_width, size_t image_height);
private:
    static void processRows(float *values);
    static void processColumns(float *values);
	
	static void processRowsOG(float* &values, size_t numberOfPixels = 64);
	static void processColumnsOG(float* &values, size_t image_width = 8, size_t image_height = 8);
};

#endif /* Arai_hpp */
