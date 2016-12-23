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
