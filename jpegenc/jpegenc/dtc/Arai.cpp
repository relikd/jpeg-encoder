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

/* Effizienz

 13 x Multiplikation
 15 x Addition
 14 x Subtraktion
  2 x Komplementbildungen
 16 x lesender Zugriff auf Array
  8 x schreibender Zugriff auf Array

 */

void Arai::transformLine(float *values) {
   
    float a0 = values[0] + values[7];
    float a1 = values[1] + values[6];
    float a2 = values[2] + values[5];
    float a3 = values[3] + values[4];
    float a5 = values[2] - values[5];
    float a6 = values[1] - values[6];
    float a7 = values[0] - values[7];
    
    float b0 = a0 + a3;
    float b1 = a1 + a2;
    float b3 = a0 - a3;
    float b4 = -(values[3] - values[4]) - a5;
    float b6 = a6 + a7;
    
    float A5_block = (b4 + b6) * A5;
    
    float d2 = ((a1 - a2) + b3) * A1;
    float d4 = -(b4 * A2) - A5_block;
    float d5 = (a5 + a6) * A3;
    float d6 = (b6 * A4) - A5_block;

    float e5 = d5 + a7;
    float e7 = a7 - d5;
    
    values[0] = (b0 + b1) * S0;
    values[1] = (e5 + d6) * S1;
    values[2] = (d2 + b3) * S2;
    values[3] = (e7 - d4) * S3;
    values[4] = (b0 - b1) * S4;
    values[5] = (d4 + e7) * S5;
    values[6] = (b3 - d2) * S6;
    values[7] = (e5 - d6) * S7;
}
