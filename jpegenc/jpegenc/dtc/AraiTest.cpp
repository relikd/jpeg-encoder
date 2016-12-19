#include "AraiTest.hpp"
#include "Test.hpp"
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
inline void stepsAll(float* &x) {
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



inline void step1(float* &x) {
	float tmp = 0;
	addSubtract(0,7);
	addSubtract(1,6);
	addSubtract(2,5);
	addSubtract(3,4);
//	tmp = x[0] - x[7];
//	x[0] += x[7];
//	x[7] = tmp;
//	tmp = x[1] - x[6];
//	x[1] += x[6];
//	x[6] = tmp;
//	tmp = x[2] - x[5];
//	x[2] += x[5];
//	x[5] = tmp;
//	tmp = x[3] - x[4];
//	x[3] += x[4];
//	x[4] = tmp;
	
}
inline void step2(float* &x) {
	float tmp = 0;
	addSubtract(0,3);
	addSubtract(1,2);
//	tmp = x[0] - x[3];
//	x[0] += x[3];
//	x[3] = tmp;
//	tmp = x[1] - x[2];
//	x[1] += x[2];
//	x[2] = tmp;
	
	x[4] += x[5];
	x[5] += x[6];
	x[6] += x[7];
}
inline void step3(float* &x) {
	float tmp = 0;
	addSubtract(0,1);
//	tmp = x[0] - x[1];
//	x[0] += x[1];
//	x[1] = tmp;
	
	x[2] += x[3];
}
inline void step4(float* &x) {
	float a5tmp = (x[4] + x[6]) * a5;
	x[2] *= a1;
	x[4] = x[4] * a2 + a5tmp;
	x[5] *= a3;
	x[6] = x[6] * a4 - a5tmp;
}
inline void step5(float* &x) {
	float tmp = 0;
	addSubtract(3,2);
	addSubtract(7,5);
//	tmp = x[3] - x[2];
//	x[2] += x[3];
//	x[3] = tmp;
//	tmp = x[7] - x[5];
//	x[5] += x[7];
//	x[7] = tmp;
}
inline void step6(float* &x) {
	float tmp = 0;
	addSubtract(7,4);
	addSubtract(5,6);
//	tmp = x[7] - x[4];
//	x[4] += x[7];
//	x[7] = tmp;
//	tmp = x[5] - x[6];
//	x[5] += x[6];
//	x[6] = tmp;
}
inline void step7(float* &x) {
	x[0] *= s0;
	x[2] *= s2;
	float tmp = x[1] * s4;
	x[1] = x[5] * s1;
	x[5] = x[4] * s5;
	x[4] = tmp;
	tmp = x[3] * s6;
	x[3] = x[7] * s3;
	x[7] = x[6] * s7;
	x[6] = tmp;
}
#undef addSubtract
void test2(float x[8]) { // adjust existing array
	step1(x);
	step2(x);
	step3(x);
	step4(x);
	step5(x);
	step6(x);
	step7(x);
}
void test3(float x[8]) { // same as test2 but prettier with define
	stepsAll(x);
}

//  ---------------------------------------------------------------
// |
// |  Approach 2 - Modular
// |
//  ---------------------------------------------------------------
inline void addSubtract(float &in_a, float &in_b, float &out_add, float &out_sub) {
	out_add = in_a + in_b;
	out_sub = in_a - in_b;
}

void test1(float x[8]) { // switching arrays
	float y[8] = {0};
	// step 1 - set y
	addSubtract(x[0], x[7], y[0], y[7]);
	addSubtract(x[1], x[6], y[1], y[6]);
	addSubtract(x[2], x[5], y[2], y[5]);
	addSubtract(x[3], x[4], y[3], y[4]);
	// step 2 - set x
	addSubtract(y[0], y[3], x[0], x[3]); // dont forget to flip x and y each time
	addSubtract(y[1], y[2], x[1], x[2]);
	x[4] = y[4] + y[5];
	x[5] = y[5] + y[6];
	x[6] = y[6] + y[7];
	x[7] = y[7];
	// step 3 - set y
	addSubtract(x[0], x[1], y[0], y[1]);
	y[2] = x[2] + x[3];
	y[3] = x[3];
	y[4] = x[4];
	y[5] = x[5];
	y[6] = x[6];
	// step 4 - set x
	y[0] = x[0];
	y[1] = x[1];
	x[7] = (y[4] + y[6]) * a5; // use x7 temporary
	x[2] = y[2] * a1;
	x[4] = y[4] * a2 + x[7];
	x[5] = y[5] * a3;
	x[6] = y[6] * a4 - x[7];
	x[7] = y[7]; // restore previous x7 value
	// step 5 - set y
	addSubtract(x[3], x[2], y[2], y[3]); // x is flipped as in flow chart
	addSubtract(x[7], x[5], y[5], y[7]);
	y[4] = x[4];
	y[6] = x[6];
	// step 6 - set x
	x[2] = y[2];
	x[3] = y[3];
	addSubtract(y[7], y[4], x[4], x[7]);
	addSubtract(y[5], y[6], x[5], x[6]);
	// step 7 - set y
	y[0] = x[0] * s0;
	y[4] = x[1] * s4;
	y[2] = x[2] * s2;
	y[6] = x[3] * s6;
	y[5] = x[4] * s5;
	y[1] = x[5] * s1;
	y[7] = x[6] * s7;
	y[3] = x[7] * s3;
}

void AraiTest::test() {
	Test::performance([]{
		float x[8] = {0};
		test1(x);
	});
	Test::performance([]{
		float x[8] = {0};
		test2(x);
	});
	Test::performance([]{
		float x[8] = {0};
		test3(x);
	});
}
