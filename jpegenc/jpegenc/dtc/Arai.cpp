#include "Arai.hpp"
#include <math.h>

#define A1 0.707106781186547524400844362104849039284835937688474036588
#define A2 0.541196100146196984399723205366389420061072063378015444681
#define A3 0.707106781186547524400844362104849039284835937688474036588
#define A4 1.306562964876376527856643173427187153583761188349269527548
#define A5 0.382683432365089771728459984030398866761344562485627041433

#define S0 0.353553390593273762200422181052424519642417968844237018294
#define S1 0.254897789552079584470969901993921956841309245954467684863
#define S2 0.270598050073098492199861602683194710030536031689007722340
#define S3 0.300672443467522640271860911954610917533627944800336361060
#define S4 0.353553390593273762200422181052424519642417968844237018294
#define S5 0.449988111568207852319254770470944197769000863706422492617
#define S6 0.653281482438188263928321586713593576791880594174634763774
#define S7 1.281457723870753089398043148088849954507561675693672456063

/*

 Effizienz
 ---------
 13 x Multiplikation
 15 x Addition
 14 x Subtraktion
  2 x Komplementbildungen
 16 x lesender Zugriff auf Array
  8 x schreibender Zugriff auf Array

*/

void Arai::transformLine(float **x) {
	float a0 = *x[0] + *x[7];
	float a1 = *x[1] + *x[6];
	float a2 = *x[2] + *x[5];
	float a3 = *x[3] + *x[4];
	float a5 = *x[2] - *x[5];
	float a6 = *x[1] - *x[6];
	float a7 = *x[0] - *x[7];
	
	float b0 = a0 + a3;
	float b1 = a1 + a2;
	float b3 = a0 - a3;
	float b4 = -(*x[3] - *x[4]) - a5;
	float b6 = a6 + a7;
	
	float A5_block = (b4 + b6) * A5;
	
	float d2 = ((a1 - a2) + b3) * A1;
	float d4 = -(b4 * A2) - A5_block;
	float d5 = (a5 + a6) * A3;
	float d6 = (b6 * A4) - A5_block;
	
	float e5 = d5 + a7;
	float e7 = a7 - d5;
	
	*x[0] = (b0 + b1) * S0;
	*x[1] = (e5 + d6) * S1;
	*x[2] = (d2 + b3) * S2;
	*x[3] = (e7 - d4) * S3;
	*x[4] = (b0 - b1) * S4;
	*x[5] = (d4 + e7) * S5;
	*x[6] = (b3 - d2) * S6;
	*x[7] = (e5 - d6) * S7;
}

Mat Arai::transform(Mat matrix) {
	processRows(matrix.values);
	processColumns(matrix.values);
	return matrix;
}

void Arai::transformMT(float* values) {
	processRows(values);
	processColumns(values);
}

void Arai::processRows(float *values) {
	for (int row = 0; row < 8; ++row)
	{
		float **currentLine = new float*[8];
		
		for (int column = 0; column < 8; ++column)
		{
			currentLine[column] = &values[row * 8 + column];
		}
		transformLine(currentLine);
		
		delete[] currentLine;
	}
}

void Arai::processColumns(float *values) {
	for (int column = 0; column < 8; ++column)
	{
		float **currentLine = new float*[8];
		
		for (int row = 0; row < 8; ++row)
		{
			currentLine[row] = &values[row * 8 + column];
		}
		transformLine(currentLine);
		
		delete[] currentLine;
	}
}


#define ARAI(_0_, _1_, _2_, _3_, _4_, _5_, _6_, _7_) \
float a0 = _0_ + _7_;\
float a1 = _1_ + _6_;\
float a2 = _2_ + _5_;\
float a3 = _3_ + _4_;\
float a5 = _2_ - _5_;\
float a6 = _1_ - _6_;\
float a7 = _0_ - _7_;\
\
float b0 = a0 + a3;\
float b1 = a1 + a2;\
float b3 = a0 - a3;\
float b4 = -(_3_ - _4_) - a5;\
float b6 = a6 + a7;\
\
float A5_block = (b4 + b6) * A5;\
\
float d2 = ((a1 - a2) + b3) * A1;\
float d4 = -(b4 * A2) - A5_block;\
float d5 = (a5 + a6) * A3;\
float d6 = (b6 * A4) - A5_block;\
\
float e5 = d5 + a7;\
float e7 = a7 - d5;\
\
_0_ = (b0 + b1) * S0;\
_1_ = (e5 + d6) * S1;\
_2_ = (d2 + b3) * S2;\
_3_ = (e7 - d4) * S3;\
_4_ = (b0 - b1) * S4;\
_5_ = (d4 + e7) * S5;\
_6_ = (b3 - d2) * S6;\
_7_ = (e5 - d6) * S7;


#define ARAI_ROW(data) ARAI( data[0], data[1], data[2], data[3], data[4], data[5], data[6], data[7] );

#define ARAI_COL(data, width) ARAI( data[0], data[width], data[2 * width], data[3 * width], data[4 * width], data[5 * width], data[6 * width], data[7 * width] );


void Arai::processRowsOG(float* &values, size_t numberOfPixels) {
	float *rowPointer = &values[0];
	size_t rowRepeat = numberOfPixels / 8;
	
	while (rowRepeat--) {
		ARAI_ROW(rowPointer);
		rowPointer += 8;
	}
}

void Arai::processColumnsOG(float* &values, size_t image_width, size_t image_height) {
	float *colPointer = &values[0];
	size_t lineSkip = image_width * 7; // 7 because one line was already processed
	
	unsigned short colRepeatX;
	unsigned short colRepeatY = image_height / 8;
	while (colRepeatY--) {
		colRepeatX = image_width;
		while (colRepeatX--) {
			ARAI_COL(colPointer, image_width);
			++colPointer;
		}
		colPointer += lineSkip;
	}
}

void Arai::transformOG(float* &values, size_t image_width, size_t image_height) {
	
	if (image_width % 8 != 0 || image_height % 8 != 0) {
		fputs("Error: Arai needs an image dimension of an multiple of 8\n", stderr);
	}
	
	processRowsOG(values, image_width * image_height);
	processColumnsOG(values, image_width, image_height);
}


#define ARAI_WITH_TRANSPOSE(_INPUT_, _OUTPUT_, _width_) \
float a0 = _INPUT_[0] + _INPUT_[7];\
float a1 = _INPUT_[1] + _INPUT_[6];\
float a2 = _INPUT_[2] + _INPUT_[5];\
float a3 = _INPUT_[3] + _INPUT_[4];\
float a5 = _INPUT_[2] - _INPUT_[5];\
float a6 = _INPUT_[1] - _INPUT_[6];\
float a7 = _INPUT_[0] - _INPUT_[7];\
\
float b0 = a0 + a3;\
float b1 = a1 + a2;\
float b3 = a0 - a3;\
float b4 = -(_INPUT_[3] - _INPUT_[4]) - a5;\
float b6 = a6 + a7;\
\
float A5_block = (b4 + b6) * A5;\
\
float d2 = ((a1 - a2) + b3) * A1;\
float d4 = -(b4 * A2) - A5_block;\
float d5 = (a5 + a6) * A3;\
float d6 = (b6 * A4) - A5_block;\
\
float e5 = d5 + a7;\
float e7 = a7 - d5;\
\
_OUTPUT_[0] = (b0 + b1) * S0;\
_OUTPUT_[_width_] = (e5 + d6) * S1;\
_OUTPUT_[2 * _width_] = (d2 + b3) * S2;\
_OUTPUT_[3 * _width_] = (e7 - d4) * S3;\
_OUTPUT_[4 * _width_] = (b0 - b1) * S4;\
_OUTPUT_[5 * _width_] = (d4 + e7) * S5;\
_OUTPUT_[6 * _width_] = (b3 - d2) * S6;\
_OUTPUT_[7 * _width_] = (e5 - d6) * S7;


void araiWithInlineTranspose(float* &input, float* &output, size_t w, size_t h) {
	const unsigned short wDivided8 = w / 8;
	const unsigned short wMinus1 = w - 1;
	const unsigned long lineSkip = (w * 8) - 8;
	
	float *in = &input[w * h];
	float *out = &output[w * h];
	unsigned short yb, ya, x;
	
	yb = h / 8; // jump blockwise over rows
	while (yb--) {
		out -= lineSkip;
		
		ya = 8; // then those 8 rows per block
		while (ya--) {
			out += wMinus1;
			
			x = wDivided8; // finally process a single row
			while (x--) {
				in -= 8;
				out -= 8;
				ARAI_WITH_TRANSPOSE(in, out, w);
			}
		}
	}
}

void Arai::transformInlineTranspose(float* &values, size_t image_width, size_t image_height) {
	
	if (image_width % 8 != 0 || image_height % 8 != 0) {
		fputs("Error: Arai needs an image dimension of an multiple of 8\n", stderr);
	}
	
	float *outValues = (float*)malloc(sizeof(float) * image_width * image_height);
	
	araiWithInlineTranspose(values, outValues, image_width, image_height);
	araiWithInlineTranspose(outValues, values, image_width, image_height);
	
	free(outValues);
}

