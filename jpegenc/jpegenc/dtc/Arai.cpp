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

Mat Arai::transform(Mat input)
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

void Arai::processColumns(Mat input) {
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

void Arai::processRows(Mat input) {
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
