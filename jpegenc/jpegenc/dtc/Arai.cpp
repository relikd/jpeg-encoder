#include "Arai.hpp"
#include <math.h>

#define C1 0.980785
#define C2 0.92388
#define C3 0.83147
#define C4 0.707107
#define C5 0.55557
#define C6 0.382683
#define C7 0.19509

#define A1 0.707107
#define A2 0.541197
#define A3 0.707107
#define A4 1.306563
#define A5 0.382683

void Arai::transformLine(float *values) {
   
    float S0 = 1 / (2 * sqrt(2));
    float S1 = 1 / (4 * C1);
    float S2 = 1 / (4 * C2);
    float S3 = 1 / (4 * C3);
    float S4 = 1 / (4 * C4);
    float S5 = 1 / (4 * C5);
    float S6 = 1 / (4 * C6);
    float S7 = 1 / (4 * C7);
    
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
    float b7 = a7;
    
    float c0 = b0 + b1;
    float c1 = b0 - b1;
    float c2 = b2 + b3;
    float c3 = b3;
    float c4 = b4;
    float c5 = b5;
    float c6 = b6;
    float c7 = b7;

    float d0 = c0;
    float d1 = c1;
    float d2 = c2 * A1;
    float d3 = c3;
    float d4 = -(c4 * A2) - ((c4 + c6) * A5);
    float d5 = c5 * A3;
    float d6 = (c6 * A4) - ((c4 + c6) * A5);
    float d7 = c7;

    float e0 = d0;
    float e1 = d1;
    float e2 = d2 + d3;
    float e3 = d3 - d2;
    float e4 = d4;
    float e5 = d5 + d7;
    float e6 = d6;
    float e7 = d7 - d5;
    
    float f0 = e0;
    float f1 = e1;
    float f2 = e2;
    float f3 = e3;
    float f4 = e4 + e7;
    float f5 = e5 + e6;
    float f6 = e5 - e6;
    float f7 = e7 - e4;
    
    values[0] = f0 * S0;
    values[1] = f5 * S1;
    values[2] = f2 * S2;
    values[3] = f7 * S3;
    values[4] = f1 * S4;
    values[5] = f4 * S5;
    values[6] = f3 * S6;
    values[7] = f6 * S7;
}
