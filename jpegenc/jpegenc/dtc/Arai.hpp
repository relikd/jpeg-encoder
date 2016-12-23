#ifndef Arai_hpp
#define Arai_hpp

#include <stdio.h>
#include "Mat.hpp"

struct Arai
{
public:
    static Mat transform(Mat matrix);
    static void transformLine(float **x);
private:
    static void processRows(float *values);
    static void processColumns(float *values);
};

#endif /* Arai_hpp */
