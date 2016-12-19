#include "AraiTest.hpp"
#include <math.h>

//#define M_PI        3.14159265358979323846264338327950288   /* pi             */

#define C(k) cosf(k * M_PI / 16)
#define S(k) 1 / ( 4 * C(k) )
static const float a1 = C(4);
static const float a2 = C(2) - C(6);
static const float a3 = C(4);
static const float a4 = C(6) + C(2);
static const float a5 = C(6);
static const float s0 = 1 / ( 2 * sqrtf(2) );
static const float s1 = S(1);
static const float s2 = S(2);
static const float s3 = S(3);
static const float s4 = S(4);
static const float s5 = S(5);
static const float s6 = S(6);
static const float s7 = S(7);
#undef S
#undef C
//  ---------------------------------------------------------------
// |
// |  Approach 1 - Stepwise
// |
//  ---------------------------------------------------------------
#define addSubtract(a,b)    tmp = x[a] - x[b]; x[a] += x[b]; x[b] = tmp;
#define addSubtractInv(a,b) tmp = x[b] - x[a]; x[a] += x[b]; x[b] = tmp;
void AraiTest::transformLine(float* &x) {
	float tmp = 0;
	// step 1
	addSubtract(0,7);
	addSubtract(1,6);
	addSubtract(2,5);
	addSubtract(3,4);
	// step 2
	addSubtract(0,3);
	addSubtract(1,2);
	x[4] = -(x[4] + x[5]);
	x[5] += x[6];
	x[6] += x[7];
	// step 3
	addSubtract(0,1);
	x[2] += x[3];
	// step 4
	tmp = (x[4] + x[6]) * a5;
	x[2] *= a1;
	x[4] = -(x[4] * a2 + tmp);
	x[5] *= a3;
	x[6] = x[6] * a4 - tmp;
	// step 5
	addSubtractInv(2,3);
	addSubtractInv(5,7);
	// step 6
	addSubtractInv(4,7);
	addSubtract(5,6);
	// step 7
	x[0] *= s0;
	x[2] *= s2;
	tmp = x[1] * s4;
	x[1] = x[5] * s1;
	x[5] = x[4] * s5;
	x[4] = tmp;
	tmp = x[3] * s6;
	x[3] = x[7] * s3;
	x[7] = x[6] * s7;
	x[6] = tmp;
}



Mat AraiTest::transform(Mat input)
{
	if ( input.rows != 8 || input.cols != 8 )
	{
		std::cout << "ERROR: Malformed matrix. Expected 8X8 but got " << input.rows << "X" << input.cols << ". Skipping Arai transformation." << std::endl;
	}
	else
	{
		processColumns(input);
		processRows(input);
	}
	return input;
}

void AraiTest::processColumns(Mat input) {
	for (int y = 0; y < 8; ++y)
	{
		float* currentColumn = new float[8];
		
		// Read values
		for (int x = 0; x < 8; ++x)
		{
			currentColumn[x] = input.get(y, x);
		}
		
		// Transform values
		transformLine(currentColumn);
		
		// Write values
		for(int x = 0; x < 8; ++x)
		{
			input.set(y, x, currentColumn[x]);
		}
		
		delete[] currentColumn;
	}
}

void AraiTest::processRows(Mat input) {
	for (int x = 0; x < 8; ++x)
	{
		float* currentRow = new float[8];
		
		// Read values
		for (int y = 0; y < 8; ++y)
		{
			currentRow[y] = input.get(y, x);
		}
		
		// Transform values
		transformLine(currentRow);
		
		// Write values
		for (int y = 0; y < 8; ++y)
		{
			input.set(y, x, currentRow[y]);
		}
		
		delete[] currentRow;
	}
}
