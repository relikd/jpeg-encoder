#include "Arai.hpp"
#include <math.h>

#define A1 0.707107
#define A2 0.541197
#define A3 0.707107
#define A4 1.306563
#define A5 0.382683

#define S0 0.353553
#define S1 0.254898
#define S2 0.270598
#define S3 0.300672
#define S4 0.353553
#define S5 0.449988
#define S6 0.653282
#define S7 1.28146

void Arai::transformLine(float *values) {
   
    float x0 = values[0];
    float x1 = values[1];
    float x2 = values[2];
    float x3 = values[3];
    float x4 = values[4];
    float x5 = values[5];
    float x6 = values[6];
    float x7 = values[7];
    
    float a0 = x0 + x7;
    float a1 = x1 + x6;
    float a2 = x2 + x5;
    float a3 = x3 + x4;
    float a4 = x3 - x4;
    float a5 = x2 - x5;
    float a6 = x1 - x6;
    float a7 = x0 - x7;

    float b0 = a0 + a3;
    float b1 = a1 + a2;
    float b2 = a1 - a2;
    float b3 = a0 - a3;
    float b4 = -a4 - a5;
    float b5 = a5 + a6;
    float b6 = a6 + a7;
    
    float c0 = b0 + b1;
    float c1 = b0 - b1;
    float c2 = b2 + b3;

    float d2 = c2 * A1;
    float d4 = -(b4 * A2) - ((b4 + b6) * A5);
    float d5 = b5 * A3;
    float d6 = (b6 * A4) - ((b4 + b6) * A5);

    float e2 = d2 + b3;
    float e3 = b3 - d2;
    float e5 = d5 + a7;
    float e7 = a7 - d5;
    
    float f4 = d4 + e7;
    float f5 = e5 + d6;
    float f6 = e5 - d6;
    float f7 = e7 - d4;
    
    values[0] = c0 * S0;
    values[1] = f5 * S1;
    values[2] = e2 * S2;
    values[3] = f7 * S3;
    values[4] = c1 * S4;
    values[5] = f4 * S5;
    values[6] = e3 * S6;
    values[7] = f6 * S7;
}
